<?php
/*
Plugin Name: Easy Digital Downloads - Mangopay Gateway through Web
Plugin URL: https://github.com/aleph1888/mangopay_edd_plugin_web
Description: Mangopay Wordpress Plugin Web (MWPW): A Mangopay gateway for Easy Digital Downloads, payment through web method
Version: 0.3.14159265359
Author: enredaos.net
*/

namespace mwpw;

use MangoPay as m;


function mwpw_init() {

	//Load language file.
	load_plugin_textdomain( 'mwpw', false,  dirname(plugin_basename(__FILE__)) . '/languages/' );

}
add_action( 'init', 'mwpw\mwpw_init' );


/**
 *
 * Main plugin initialization.
 */
function mwpw_load_gateway() {

	//Define Mangopay API connection global vars importing settings.
	global $edd_options;
	define('MWPW_temp_path', $edd_options[ 'mwpw_temp_folder']);

	if ( edd_is_test_mode() ) {
		define('MWPW_client_id', $edd_options[ 'mwpw_test_api_user']);
		define('MWPW_password', $edd_options[ 'mwpw_test_api_key']);
		define('MWPW_base_path', 'https://api.sandbox.mangopay.com');
	} else {
		define('MWPW_client_id', $edd_options[ 'mwpw_live_api_user']);
		define('MWPW_password', $edd_options[ 'mwpw_live_api_key']);
		define('MWPW_base_path', 'https://api.mangopay.com');
	}

	//Load plugin files
	require_once __DIR__ . "/includes/mwpw_api.inc";
	require_once __DIR__ . "/includes/mwpw_user.inc";
	require_once __DIR__ . "/includes/mwpw_wallet.inc";
	require_once __DIR__ . "/includes/mwpw_bank.inc";
	require_once __DIR__ . "/includes/mwpw_fields.inc";
	require_once __DIR__ . "/includes/mwpw_forms.inc";
	require_once __DIR__ . "/includes/mwpw_errors.inc";


	//Configuration of Mangopay user/bankAccount info in profile section.
	include (__DIR__ . "/mwpw_profile.php");

	//Withdraw metabox in post edition sidebar.
	include (__DIR__ . "/mwpw_post.php");

	//Register gateway in EDD if settings are setted
	include (__DIR__ . "/mwpw_gateway.php");

	mwpw_init_site();

}
add_action( 'plugins_loaded', 'mwpw\mwpw_load_gateway' );


/**
 *
 * Returns array with admin users for settings type-select options.
 *
 * @return array
 */
function mwpw_get_site_admin_array() {

	$admins = get_users( array ( 'role' => 'administrator' ) );

	$yAdmins[''] = '--';
	foreach ( $admins as $admin ) {
		$yAdmins[$admin->ID] = $admin->user_login;
	}

	return $yAdmins;

}


/**
 *
 * Returns echoable string with base user and wallet configuration, retrieving site admin id's;
 * [see mwpw_gateway.php/mwpw_init_site()]
 *
 * @return string
  */
function mwpw_get_current_site_MangoPay_config() {

	global $edd_options;

	$wp_user = get_userdata ( $edd_options['mwpw_site_admin_wp_id'] );

	$u_id = $edd_options['mwpw_mangopay_wallets_owner_id'];
	$u_id = ( $u_id ? $u_id : '0' );

	$w_id = $edd_options['mwpw_mangopay_site_wallet_id'];
	$w_id = ( $w_id ? $w_id : '0' );

	if ( $w_id ) {
		$m_wallet_id = $w_id;
		$owner_id = $u_id;
		$owner_type = 'user';
		$context_error = "init";
		$mwpw_wallet = new mwpw_wallet( $m_wallet_id, $owner_id, $owner_type, $context_error );
		$mwpw_wallet->mwpw_load_wallet();
	}

	if ( isset( $mwpw_wallet->m_wallet ) )
		$w_amount = $mwpw_wallet->m_wallet->Balance->Amount / 100;
	else
		$w_amount = '0';

	$w_amount .= __( ' eur', 'mwpw' );
	$str_output = "<br>MangoPay\User ID: %s <br>MangoPay\Wallet ID: %s <br>". __( 'Loosed amount: ', 'mwpw' ) . " %s";
	return sprintf( $str_output, $u_id, $w_id, $w_amount );

}


/**
 *
 * Manage Downloads/Settings/Gateways plugin fields.
 *
 * @params array $settings
 *
 * @return array
 */
function mwpw_add_settings( $settings ) {

	//Prepare setting 'mwpw_site_admin_wp_id' array
	global $edd_options;
	$str_site_MangoPay = mwpw_get_current_site_MangoPay_config();

	//Load array
	$mangopay_gateway_settings = array(
		array(
			'id' => 'mwpw_mangopay_gateway_settings',
			'name' =>  __( 'Mangopay Gateway Settings.', 'mwpw' ),
			'type' => 'header'
		),
		array(
			'id' => 'mwpw_site_admin_wp_id',
			'name' => '<strong>' . __( 'Site Admin', 'mwpw' ) . '</strong>',
			'desc' => __( 'Select site admin', 'mwpw') . "\n{$str_site_MangoPay}",
			'type' => 'select',
			'options' => mwpw_get_site_admin_array(),
			'std' => $edd_options['mwpw_site_admin_wp_id']
		),
		array(
			'id' => 'mwpw_live_api_user',
			'name' => __( 'Live API User', 'mwpw' ),
			'desc' => __( 'Enter your live API user', 'mwpw' ),
			'type' => 'text',
			'size' => 'regular'
		),
		array(
			'id' => 'mwpw_live_api_key',
			'name' => __( 'Live API Key', 'mwpw' ),
			'desc' => __( 'Enter your live API key', 'mwpw' ),
			'type' => 'text',
			'size' => 'regular'
		),
		array(
			'id' => 'mwpw_test_api_user',
			'name' => __( 'Test API User', 'mwpw' ),
			'desc' => __( 'Enter your test API user', 'mwpw' ),
			'type' => 'text',
			'size' => 'regular'
		),
		array(
			'id' => 'mwpw_test_api_key',
			'name' => __( 'Test API Key', 'mwpw' ),
			'desc' => __( 'Enter your test API key', 'mwpw' ),
			'type' => 'text',
			'size' => 'regular'
		),
		array(
			'id' => 'mwpw_temp_folder',
			'name' => __( 'Temp folder', 'mwpw' ),
			'desc' => __( 'Enter a temp folder where Mangopay API can write', 'mwpw' ),
			'type' => 'text',
			'size' => 'regular'
		),
		array(
			'id' => 'mwpw_conditions_rules_image',
			'name' => __( 'Conditions rules image', 'mwpw' ),
			'desc' => __( 'Enter the url where you have uploaded MangoPay logo', 'mwpw' ),
			'type' => 'text',
			'size' => 'regular'
		),
		array(
			'id' => 'mwpw_conditions_rules',
			'name' => __( 'Conditions & rules', 'mwpw' ),
			'desc' => __( 'Enter the url where you have uploaded MangoPay conditions', 'mwpw' ),
			'type' => 'text',
			'size' => 'regular',
			'std'  => 'http://www.mangopay.com/wp-content/blogs.dir/10/files/2013/11/CGU-API-MANGOPAY-MAI-2013_ESP.pdf'
		)
	);

	return array_merge( $settings, $mangopay_gateway_settings );

}
add_filter( 'edd_settings_gateways', 'mwpw\mwpw_add_settings' );


/**
 *
 * Reads errors and display any existing on key 'init' for admin users, after it will clean stack.
 *
 * @uses mwpw_errors class
 * @uses add_settings_error, settings_errors
 */
function mwpw_admin_notices() {

	//Print any init errors
	if ( mwpw_errors::mwpw_errors_get( 'init' ) && wp_get_current_user()->roles[0] == 'administrator' ) {

		$plugin_name = __( 'Easy Digital Downloads - Mangopay Gateway through Web: ', 'mwpw' ) . "<br>";

		add_settings_error( 'mwpwinit', '', $plugin_name . mwpw_errors::mwpw_errors_get( 'init' ) , 'error' );
		mwpw_errors::mwpw_errors_clean( 'init' );

		settings_errors( 'mwpwinit', false ) ;
	}

}
add_action( 'admin_notices', 'mwpw\mwpw_admin_notices' );

/**
 *
 * Listens for EDDlistener with $_GET['mangopaywebBankErase]
 * Requires $_GET['u'] corresponding with WP_user that has a bank_id that needs to erase
 */
function mwpw_listen_for_mangopayweb_BankErase() {

	if ( isset( $_GET['edd-listener'] ) && $_GET['edd-listener'] == 'mangopaywebBankErase' ) {

		$WP_user_id =  $_GET["u"];

		//Only admin or own user can perform action
		if ( wp_get_current_user()->ID == $WP_user_id || wp_get_current_user()->roles[0] == 'administrator' ) {
			update_user_meta( $WP_user_id, 'bank_id', null );
			wp_redirect( get_edit_user_link( $WP_user_id ) ) ;
			die;
		}

	}

}
add_action( 'init', 'mwpw\mwpw_listen_for_mangopayweb_BankErase' );


/**
 *
 * Listens for EDDlistener with $_GET['mangopaywebUserErase]
 * Requires $_GET['u'] corresponding with WP_user that has a user_id needed to erase;
 * bank and wallet id's will be also deleted.
 */
function mwpw_listen_for_mangopayweb_UserErase() {

	if ( isset( $_GET['edd-listener'] ) && $_GET['edd-listener'] == 'mangopaywebUserErase' ) {

		$WP_user_id =  $_GET["u"];

		//Only admin or own user can perform action
		if ( wp_get_current_user()->ID == $WP_user_id || wp_get_current_user()->roles[0] == 'administrator' ) {
			update_user_meta( $WP_user_id, 'mangopay_id', null );
			update_user_meta( $WP_user_id, 'bank_id', null );
			update_user_meta( $WP_user_id, 'wallet_id', null );
			wp_redirect( get_edit_user_link( $WP_user_id ) );
			die;
		}

	}

}
add_action( 'init', 'mwpw\mwpw_listen_for_mangopayweb_UserErase' );


/**
 *
 * Listens for EDDlistener with $_GET['mangopaywebWalletErase]
 * Requires $_GET['u'] corresponding with WP_user that has a wallet_id needed to erase or...
 * ... requires $_GET['p'] corresponding with WP_post that has a wallet_id neeede to erase.
 * Requieres $_GET['r'] which means the referer.
 */
function mwpw_listen_for_mangopayweb_WalletErase( ) {

	if ( isset( $_GET['edd-listener'] ) && $_GET['edd-listener'] == 'mangopaywebWalletErase' ) {

		$wp_user_id =  $_GET["u"];
		$wp_post_id =  $_GET["p"];
		$str_referer = urldecode(  $_GET["r"] );

		//Only administrators or related user can perform action
		if ( $WP_user_id )  {

			if ( current_user_can('edit_user', $wp_user_id ) )
				update_user_meta( $wp_user_id, 'wallet_id', 0 );

		} elseif ( $WP_post_id ) {

			if ( current_user_can('edit_post', $wp_post_id ) ) {
				update_post_meta( $wp_post_id, 'wallet_id', 0 );
				update_post_meta( $wp_post_id, 'payout_id', 0 );
			}

		} else {

			$str_referer = trailingslashit( home_url( 'index.php' ) );
		}

		wp_redirect( $str_referer );
		die;

	}

}
add_action( 'init', 'mwpw\mwpw_listen_for_mangopayweb_WalletErase' );


/**
*
* Comming from mwpw_payout::mwpw_bankaccount_get_info() payout button,
* processes payout request and redirects to edition post page.
*/
function mwpw_listen_for_mangopayweb_payout_order() {

	if ( isset( $_GET['edd-listener'] ) && $_GET['edd-listener'] == 'mangopaywebPAYOUT' ) {

		// get post identificator
		$wp_post_id = $_GET['pid'];
		$wp_post = get_post( $wp_post_id );

		// gatekeeper
		$wp_user = get_userdata( $wp_post->post_author );
		if ( ! current_user_can( 'edit_post', $wp_post_id)  ) {
			wp_redirect ( site_url () );
			die;
		}

		// build payout
		$m_wallet_id = $wp_post->wallet_id;
		$owner_id = $wp_post_id;
		$owner_type = 'post';
		$context_error = "payout";
		$mwpw_wallet = new mwpw_wallet( $m_wallet_id, $owner_id, $owner_type, $context_error );
		$mwpw_wallet->mwpw_load_wallet();
		require_once __DIR__ . "/includes/mwpw_payout.inc";
		$m_payout = mwpw_payout::mwpw_do_payout( $mwpw_wallet->m_wallet, $wp_user->bank_id );

		// notify and save created id
		if ( $m_payout->Status ) {
			mwpw_errors::mwpw_errors_append( 'payout', __( "Payout operation has been ordered with result: ", 'mwpw' ) . $m_payout->Status );

			if ( $m_payout->Status == "CREATED" ) {
				update_post_meta( $wp_post_id, 'payout_id', $m_payout->Id );
			}
		}

		// return to post
		wp_redirect ( get_edit_post_link( $wp_post_id, '&' ) );
		die;

	}
}
add_action( 'init', 'mwpw\mwpw_listen_for_mangopayweb_payout_order' );
