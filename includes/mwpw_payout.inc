<?php

/**
 * Payout
 *
 * This class is for manage Mangopay payout process
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

use MangoPay as m;

class mwpw_payout {

	/**
	*
	* Fetch MangoPay\PayOut
	*
	* @params integer $m_payout_id MangoPay\PayOut id
	*
	* @return MangoPay\PayOut
	*/
	static public function mwpw_get_payout(	$m_payout_id ) {

		try {
			if ( $m_payout_id )
				return mwpw_api::get_instance()->PayOuts->Get( $m_payout_id );

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api ( $context_error, $e, __( 'Fetching PayOut', 'mwpw') );

		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage ( $context_error, $e, __( 'Fetching PayOut', 'mwpw') );
		}

		return Null;

	}


	/**
	* Retrieves nice <div> block showing bank withdraw info and withdraw button.
	*
	* @param WP_Post $wp_post This is a post that have a field containing MangoPay/Wallet object ID.
	*/
	public static function mwpw_bankaccount_get_info ( $wp_post ) {

		$api = mwpw_api::get_instance();

		$wp_user = get_userdata( $wp_post->post_author );

		if ( ! $wp_user->mangopay_id ) {
			$must_fill_user = __( "In order to request for a withdraw of your funds, author %s must fill MangoPay User form on profile.", 'mwpw' );
			return sprintf( $must_fill_user, $wp_user->user_nicename );
		}

		if ( ! $wp_user->bank_id ) {
			$must_fill_bankdata = __( "In order to request for a withdraw of your funds, author %s must fill MangoPay Bank form on profile.", 'mwpw' );
			return sprintf( $must_fill_bankdata, $wp_user->user_nicename );
		}

		try {

			//Get bankAccountInfo
			$bankAccountResult = $api->Users->GetBankAccount( $wp_user->mangopay_id, $wp_user->bank_id );
			$output = mwpw_forms::mwpw_show_bank_section_info ( $bankAccountResult );

			//Set payout process url through EDD listener
			$params = "pid={$wp_post->ID}";
			$listener_url = trailingslashit( home_url( 'index.php' ) ) . "?edd-listener=mangopaywebPAYOUT&{$params}";

			//Return submit button
			$caption = __( 'Order withdraw', 'mwpw');
			$output  .= "<br>";
			$output  .= "<a href={$listener_url} class='button'>{$caption}</a>";
			return $output;

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api( 'payout', $e, __( 'Retrieving bank account info', 'mwpw') );

		} catch ( m\Exception $e ) {

			mwp_errors::mwpw_errors_manage( 'payout', $e, __( 'Retrieving bank account info', 'mwpw') );

		}
	}

	/**
	* Creates a MangoPay\Payout object related to wallet with all of its funds.
	*
	* @param integer $m_wallet MangoPay\Wallet
	* @param integer $m_bank_id MangoPay\BankAccount id
	*
	* @return MangoPay\Payout
	*/
	static public function mwpw_do_payout( $m_wallet, $m_bank_id ) {

		$payout = new m\PayOut();

		$payout->DebitedWalletId = $m_wallet->Id;
		$payout->PaymentType = 'BANK_WIRE';
		$payout->AuthorId = $m_wallet->Owners[0];

		$payout->DebitedFunds = $m_wallet->Balance;

		$money_fees = new m\Money();
		$money_fees->Amount = 0;
		$money_fees-> Currency = 'EUR';
		$payout->Fees = $money_fees;

		$payoutdetails = new m\PayOutPaymentDetailsBankWire();
		$payoutdetails->BankAccountId = $m_bank_id;

		$payout->MeanOfPaymentDetails = $payoutdetails;

		try {

			$newPayout = mwpw_api::get_instance()->PayOuts->Create($payout);
			return $newPayout;

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api ( 'payout', $e,  __( 'Ordering payout', 'mwpw') );

		} catch ( m\Exception $e ) {

			mwp_errors::mwpw_errors_manage ( 'payout', $e, __( 'Ordering payout', 'mwpw') );
		}

		return $payout;
	}
}

?>
