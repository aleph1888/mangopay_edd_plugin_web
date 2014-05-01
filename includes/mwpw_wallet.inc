<?php

/**
 * wallet
 *
 * Extends MangoPay Wallet class and exposes some util methods
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 *
 * @uses 	mwpw_error
 */

namespace mwpw;

use MangoPay as m;

/**
 * Notice:
 * By configuring $wallets_owner_id to $edd_options['mwpw_mangopay_wallets_owner_id'],
 * only one MangoPay\User will own all the wallets.
 * Consider to use proper param in construct if you want to change this clause.
 */

class mwpw_wallet extends m\Wallet {

	public $m_wallet_id;		//integer MangoPay\Wallet id
	public $m_wallet;		//MangoPay\Wallet

	public $owner_id;		//integer WP_user or WP_post id owning wallet in field: wallet_id
	private $owner_type;		//string Valid values: { 'user', 'post' }

	private $wallets_owner_id;	//integer MangoPay\User (legal or natural)

	private $context_error;		//string Key where errors will be hold

	function __construct( $m_wallet_id, $owner_id, $owner_type, $context_error = "wallet", $wallets_owner_id = NULL ) {

		$this->m_wallet_id = $m_wallet_id;
		$this->owner_id = $owner_id;
		$this->owner_type = $owner_type;

		$this->context_error = $context_error;

		if ( $wallets_owner_id ) {
			$this->wallets_owner_id = $wallets_owner_id;
		} else {
			global $edd_options;
			$this->wallets_owner_id = $edd_options['mwpw_mangopay_wallets_owner_id'];
		}

	}


	/**
	*
	* Fetch MangoPay\Wallet
	*
	* Could happen a bad MangoPay Id is configured (ie: sandbox to production), then a button to delete will be provided.
	* Localize error in stack by 'Fetching wallet' key.
	*
	* @params string $str_listerner_url If wallet not found, will redirect to this listener in order to erase it.
	* @params bool $report_missing Ignore listener if setted to false
	* @params string $context_error Any error will be added to stack using this key
	*
	* @return MangoPay\Wallet
	*
	* @uses mwpw_print
	* @uses mwpw_errors
	*/
	private function mwpw_get_wallet(
					$str_listerner_url = NULL,
					$report_missing = TRUE
				) {

		try {
			if ( $this->m_wallet_id )
				$m_wallet = mwpw_api::get_instance()->Wallets->Get( $this->m_wallet_id );

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_entity_not_found(
									$e,
									$this->m_wallet_id,
									$report_missing,
									$listener_url,
									$this->context_error,
									__( 'Getting wallet', 'mwpw')
								);

		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage ( $this->context_error, $e, __( 'Getting wallet', 'mwpw') );

		}

		return $m_wallet;

	}


	/**
	*
	* Fetches MangoPay\Wallet related
	*
	* @param bool $create if not exits
	* @param bool $report_missing
	*
	* @return mwpw_wallet
	*/
	function mwpw_load_wallet( $create = false, $report_missing = true ) {

		switch( $this->owner_type ) {
			case 'post':
				$str_listener_url = trailingslashit( home_url( 'index.php' ) ) . '?edd-listener=mangopaywebWalletErase';
				$str_listener_url .= "&p={$this->owner_id}";
				$referer = get_edit_post_link( $this->owner_id, '&' );
				$str_listener_url .= "&r={$referer}";
				$str_description = get_the_title( $this->owner_id );
				break;
			case 'user':
				$str_listener_url = trailingslashit( home_url( 'index.php' ) ) . '?edd-listener=mangopaywebWalletErase';
				$str_listener_url .= "&u={$this->owner_id}";
				$referer = urlencode( network_site_url( '/' ) . 'wp-admin/edit.php?post_type=download&page=edd-settings&tab=gateway' );
				$str_listener_url .= "&r={$referer}";
				$str_description = get_userdata($this->owner_id)->user_nicename;
				break;
		}

		$this->m_wallet = $this->mwpw_get_wallet( $str_wallet_listener, $report_missing );

		if ( $create && ! isset( $this->m_wallet ) )
			$this->m_wallet = $this->mwpw_create_wallet( $str_description );

		if ( $this->m_wallet )
			$this->m_wallet_id = $this->m_wallet->Id;

	}


	/**
	*
	* Creates a MangoPay\Wallet over $this->$m_wallet;
	*
	* @param string $str_description
	*/
	function mwpw_create_wallet( $str_description ) {

		try {

			$m_wallet = new m\Wallet();
			$m_wallet->Owners = array( $this->wallets_owner_id );
			$m_wallet->Currency = 'EUR';
			$m_wallet->Description = $str_description;
			$m_wallet = mwpw_api::get_instance()->Wallets->Create($m_wallet);

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api( $this->context_error, $e, __( 'Creating wallet', 'mwpw') );

			if ( $this->wallets_owner_id ) {
				$maybe_wrong_owner = __( 'If wrong Id is %s, visit %s profile to fix it. You will get a message with proper link.', 'mwpw' );
				mwpw_errors::mwpw_errors_append( $this->context_error, sprintf(
									$maybe_wrong_owner,
									$this->wallets_owner_id,
									get_userdata($this->owner_id)->user_nicename
									)
								);
			}

		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage( $this->context_error, $e, __( 'Creating wallet', 'mwpw') );

		}

		return $m_wallet;

	}

}

?>
