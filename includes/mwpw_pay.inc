<?php

/**
 * Pay
 *
 * This class is for manage Mangopay pay process
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

use MangoPay as m;

class mwpw_pay {

	public $mwpw_user;		//@param mwpw_user debited MangoPay\User (legal or natural)

	private $m_wallets_owner_id; 	//see mwpw_gateways.php/mwpw_init_site()
	private $m_site_wallet_id;  	//see mwpw_gateways.php/mwpw_init_site()


	/**
	 *
	 * @params object $ob_user Debited user object with (at least) two field: ob_id and mangopay_id
	 * @params bool $autosave Creates a new MangoPay\User from $_POST data.
	 * @params string $str_listener_url If bad MangoPay identificator will provide url to fix. See mwpw_user construct.
	 * @params integer $wallets_owner_id
	 * @params integer $site_wallet_id
	 */
	public function __construct( $ob_user, $autosave = FALSE, $str_listener_url, $m_wallets_owner_id = NULL, $m_site_wallet_id = NULL ) {

		// fetch user
		$this->mwpw_user = new mwpw_user( $ob_user, $autosave, $str_listener_url );

		// set site owner and wallet
		global $edd_options;
		$this->m_wallets_owner_id = ( $m_wallets_owner_id ? $m_wallets_owner_id : $edd_options['mwpw_mangopay_wallets_owner_id'] );
		$this->m_site_wallet_id = ( $m_site_wallet_id ? $m_site_wallet_id : $edd_options['mwpw_mangopay_site_wallet_id'] );

	}


	/**
	*
	* Creates a MangoPay\PayIn with ExecutionDetails as MangoPay\PayInExecutionDetailsWeb
	*
	* @param float (2 decimal) $amount
	* @param float (2 decimal) $fees
	* @param string $return_url Listener that will callback payment after card validation
	* @param integer $edd_payment_id
	*
	* @return string MangoPay validation card server url for created PayIn
	*/
	public function mwpw_payIn( $amount, $fees, $return_url, $edd_payment_id ) {

		$payIn = new m\PayIn();

		global $edd_options;
		$debited_user = get_userdata( $edd_options['mwpw_site_admin_wp_id'] );

		$payIn->AuthorId = $this->mwpw_user->mangopay_id;
		$payIn->DebitedFunds = array(
				'amount' => (int) $amount * 100 ,
				'currency' => 'EUR'
		);
		$payIn->Fees = array(
				'amount' => (int) $fees * 100 ,
				'currency' => 'EUR'
		 );
		$payIn->Tag = $edd_payment_id;

		$payIn->CreditedUserId = $this->m_wallets_owner_id;
		$payIn->CreditedWalletId = $this->m_site_wallet_id;

		$payIn->Culture="ES";

		$payIn->PaymentDetails = new m\PayInPaymentDetailsCard();
		$payIn->PaymentDetails->CardType = 'CB_VISA_MASTERCARD';

		$payIn->ExecutionType = 'WEB';
		$payIn->ExecutionDetails = new m\PayInExecutionDetailsWeb();
		$payIn->ExecutionDetails->SecureModeReturnURL = "";
		$payIn->ExecutionDetails->ReturnURL = $return_url;

		try {

			$api = mwpw_api::get_instance();
			$payInResult = $api->PayIns->Create($payIn);

			if ( isset( $payInResult ) && isset( $payInResult->Id ) )
				return $payInResult->ExecutionDetails->RedirectURL;

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api( 'gateway', $e, __( 'Creating payment', 'mwpw' ) );

		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage( 'gateway', $e,  __( 'Creating payment', 'mwpw' ) );

		}

		return null;
	}


	/**
	*
	* Returns MangoPay\PayIn or redirects to checkout form if error
	*
	* @param integer $transactionId
	*
	* @return MangoPay\PayIn
	*/
	static public function mwpw_fetch_payin( $transactionId ) {

		try {

			$api = mwpw_api::get_instance();
			$createdPayIn = $api->PayIns->Get( $transactionId );

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api( 'gateway', $e, __( 'Getting PayIn', 'mwpw') );
			edd_send_back_to_checkout( '?payment-mode=mangopayweb' );

		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage( 'gateway', $e,  __( 'Getting PayIn', 'mwpw') );
			edd_send_back_to_checkout( '?payment-mode=mangopayweb' );

		}

		return $createdPayIn;

	}


	/**
	*
	* Will transfer funds from $edd_options[ 'mwpw_mangopay_site_wallet_id' ] to post_type='download' wallets
	* according to edd_payment cart. Create wallet for post if unexisting.
	*
	* @param integer $edd_payment_id
	*/
	public function mwpw_distribute_funds( $edd_payment_id ) {

		//Get all items in cart
		$edd_payments = edd_get_payment_meta_cart_details( $edd_payment_id, FALSE );

		//Process redistribution to wallets
		foreach ( $edd_payments as $item ) {

			//Read item data
			$wp_post_id = $item["id"];
			$amount = (int) $item["item_price"] * 100;

			//Fetch related post
			$wp_post = get_post( $wp_post_id );
			$wp_user = get_userdata( $wp_post->post_author );

			//Get wallet (creates a new one if first payment to this post)
			$m_wallet_id = $wp_post->wallet_id;
			$owner_id = $wp_post_id;
			$owner_type = 'post';
			$context_error = "IPN";
			$credited_wallet = new mwpw_wallet( $m_wallet_id, $owner_id, $owner_type, $context_error );
			$create = TRUE;
			$credited_wallet->mwpw_load_wallet( $create );

			//Update post wallet_id field if new one has been generated
			if ( $m_wallet_id != $credited_wallet->m_wallet_id )
				update_post_meta( $wp_post_id, 'wallet_id', $credited_wallet->m_wallet_id );

			$transaction = new m\Transfer();

			$transaction->AuthorId =  $this->m_wallets_owner_id;
			$transaction->CreditedUserId = $credited_wallet->Owners[0];

			$transaction->DebitedWalletId = $this->m_site_wallet_id;
			$transaction->CreditedWalletId= $credited_wallet->m_wallet_id;

			$transaction->DebitedFunds = new m\Money();
			$transaction->DebitedFunds->Amount = $amount * 100;
			$transaction->DebitedFunds->Currency = 'EUR';
			$transaction->Fees = $createdPayIn->Fees;

			try {
				$api = mwpw_api::get_instance();
				return $api->Transfers->Create( $transaction );

			} catch ( m\ResponseException $e ) {

					mwpw_errors::mwpw_errors_manage_api( 'IPN', $e, __( 'Transferring payment to wallets', 'mwpw' ) );

			} catch ( m\Exception $e ) {

					mwpw_errors::mwpw_errors_manage( 'IPN', $e, __( 'Transferring payment to wallets', 'mwpw' ) );

			}
		}
	}
}
?>
