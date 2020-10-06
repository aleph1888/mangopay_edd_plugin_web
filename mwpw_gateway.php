<?php

/**
 * Gateway
 *
 * This file manages MangoPay Payment through web, hooking EDD Gateways required actions and filters.
 * Getinfo on: http://pippinsplugins.com/create-custom-payment-gateway-for-easy-digital-downloads/
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

use MangoPay as m;

/**
 *
 * Registers the gateway in EDD system; filter for edd_payment_gateways()
 * Will be triggered after mwpw_init_site()
 *
 * @params array $gateways
 *
 * @return array
 */
function mwpw_register_gateway( $gateways ) {

	$gateways['mangopayweb_gateway'] = array(
				'admin_label' => 'MangopayWeb',
				'checkout_label' => __( 'Mangopay', 'mwpw' )
				);
	return $gateways;

}

/**
 *
 * Checks if a 'Site admin' has been choosen in wp-admin|Downloads|Settings|Gateways|Mangopay Gateway Settings section,
 * so global $edd_options['mwpw_site_admin_wp_id'] is setted.
 *
 * Also checks if this WP_user has filled his Profile MangoPay's form and has a valid MangoPay\User,
 * so $edd_options['mwpw_mangopay_wallets_owner_id'] field is setted. Reports if missing via
 * WP::add_settings_error().
 *
 * Also checks if $edd_options['mwpw_mangopay_site_wallet_id'] is setted authored by previous user;
 * Creates new if not. All payments will go to this wallet, and then will be transfered to Posts wallets
 * after IPN callback.
 *
 * NOTICE: By default, any generated wallet will be authored by this user. But this can be overwritten in constructor.
 *
 * @return bool Could init or not.
 */
function mwpw_init_site() {

	global $edd_options;
	$wp_admin = get_userdata( $edd_options['mwpw_site_admin_wp_id'] );

	$edd_options['mwpw_mangopay_wallets_owner_id'] = 0;
	$edd_options['mwpw_mangopay_site_wallet_id'] = 0;

	$gateway_not_available = __( 'Gateway is not available until it is fully configured. <br>', 'mwpw' );

	//Site user
	$WP_user_setted = isset( $wp_admin->user_login );
	if ( ! $WP_user_setted  ) {

		$site_admin_required = $gateway_not_available .
					__( 'Must configure site admin in Downloads/Settings/Gateways/MangoPay section' );
		mwpw_errors::mwpw_errors_add( 'init', $site_admin_required );

	}

	//MangoPay User
	$MangoPay_user_setted = isset( $wp_admin->mangopay_id );
	if ( $WP_user_setted && $MangoPay_user_setted ) {
		$edd_options['mwpw_mangopay_wallets_owner_id'] = $wp_admin->mangopay_id;
	} else {
		$user_invalid_mangopay_id = $gateway_not_available .
					__( 'Must configure MangoPay User section in %s profile.', 'mwpw' );
		mwpw_errors::mwpw_errors_add ( 'init', sprintf( $user_invalid_mangopay_id, "<i>{$wp_admin->user_login}</i>" ) );
	}

	if ( $WP_user_setted && $MangoPay_user_setted ) {

		//Wallet
		$m_wallet_id = $wp_admin->wallet_id;
		$owner_id = $wp_admin->ID;
		$owner_type = 'user';
		$context_error = "init";
		$mwpw_wallet = new mwpw_wallet( $m_wallet_id, $owner_id, $owner_type, $context_error );

		$create_wallet = TRUE;
		$mwpw_wallet->mwpw_load_wallet( $create_wallet );

		//If everything went ok, register gateway...
		if ( isset( $mwpw_wallet->m_wallet_id ) ) {

			//Set values
			if ( $mwpw_wallet->m_wallet_id != $wp_admin->ID )
				update_user_meta( $wp_admin->ID, 'wallet_id', $mwpw_wallet->m_wallet_id );

			$edd_options['mwpw_mangopay_site_wallet_id'] = $mwpw_wallet->m_wallet_id;

			//Register gateway
			add_filter( 'edd_payment_gateways', 'mwpw\mwpw_register_gateway' );
			return TRUE;
		}
	}

	return FALSE;

}

/**
 *
 * As credit card validation is done on Mangopay server,
 * Will use cc_form to register guest users in MangoPay system as payments cannot be done without a user,
 * or if user exists, will provide updating data form.
 * TODO: Register in wordpress and send to user a mail with his login credentials or implement EDD autoregistration
 * https://easydigitaldownloads.com/docs/custom-checkout-fields/
 */
function mwpw_mangopayweb_gateway_cc_form() {

	require_once ( __DIR__ . "/includes/mwpw_pay.inc");
	require_once ( __DIR__ . "/includes/mwpw_api.inc");
	require_once ( __DIR__ . "/includes/mwpw_errors.inc");

	//Retrieves user through mwpw_pay object. If not existing will be saved in mwpw_edd_process_payment()
	$wp_user = wp_get_current_user();
	$autosave = false;
	$str_listener_url = trailingslashit( home_url( 'index.php' ) ) . '?edd-listener=mangopaywebUserErase';
	$str_listener_url .= "&u={$wp_user->ID}";
	$po = new mwpw_pay( $wp_user, $autosave, $str_listener_url );

	//Display User form
	$output .= '<div>';

		//Ask for register (if user don't, we don't keep information, but do the process creating a user in MangoPay)
		$output .= mwpw_forms::mwpw_show_wordpress_login();

		//Display user data
		$output .= mwpw_forms::mwpw_show_user_section( $po->mwpw_user, ! $po->mwpw_user->mangopay_id );

		//Display conditions and rules
		global $edd_options;
		$img_logo_url = $edd_options['mwpw_conditions_rules_image'];
		$doc_logo_url = $edd_options['mwpw_conditions_rules'];
		$output .= mwpw_print_link ("<img src='{$img_logo_url}'><br>" . __( 'General conditions', 'mwpw' ), $doc_logo_url );

		//Hide post_type=download id where paying is done
		$output .=  "<input type='hidden' name ='mwp_post_id' value='{$_REQUEST['mwpw_post_id']}'>";

	$output .= "</div>";

	echo $output;

}
add_action( 'edd_mangopayweb_gateway_cc_form', 'mwpw\mwpw_mangopayweb_gateway_cc_form');

/**
 *
 * Create a MangoPay\PayIn object and redirect to MangoPay card validation web server.
 * Set a listener to get result callback.
 * Will retrieve PayIn object through $_GET['TransactionId'] and will retrieve EDD_paymentID trough PayIn->Tag field
 *
 * @param array $purchase_data
 */
function mwpw_process_payment( $purchase_data ) {

	/**********************************
	* Purchase data comes in like this:

	    $purchase_data = array(
        	'downloads'     => array of download IDs,
	        'tax' 		=> taxed amount on shopping cart
	        'fees' 		=> array of arbitrary cart fees
	        'discount' 	=> discounted amount, if any
	        'subtotal'	=> total price before tax
	        'price'         => total price of cart contents after taxes,
	        'purchase_key'  =>  // Random key
	        'user_email'    => $user_email,
	        'date'          => date( 'Y-m-d H:i:s' ),
	        'user_id'       => $user_id,
        	'post_data'     => $_POST,
	        'user_info'     => array of user's information and used discount code
	        'cart_details'  => array of cart details,
	);
    	*/

	//Check for any stored errors
	$errors = edd_get_errors();
	if ( ! $errors ) {

		$purchase_summary = edd_get_purchase_summary( $purchase_data );

		/****************************************
		* setup the payment details to be stored
		****************************************/

		$payment = array(
			'price'        => $purchase_data['price'],
			'date'         => $purchase_data['date'],
			'user_email'   => $purchase_data['user_email'],
			'purchase_key' => $purchase_data['purchase_key'],
			'currency'     => $edd_options['currency'],
			'downloads'    => $purchase_data['downloads'],
			'cart_details' => $purchase_data['cart_details'],
			'user_info'    => $purchase_data['user_info'],
			'status'       => 'pending'
		);

		// record the pending payment
		$payment = edd_insert_payment( $payment );

		$merchant_payment_confirmed = false;

		$error = false;

		// instantiate gateway object
		require_once ( __DIR__ . "/includes/mwpw_pay.inc");
		$autosave = true;
		$str_listener_url = trailingslashit( home_url( 'index.php' ) ) . '?edd-listener=mangopaywebUserErase';
		$str_listener_url .= "&u={$wp_user->ID}";
		$po = new mwpw_pay( wp_get_current_user(), $autosave, $listener_url ) ;

		//Verify user. Display errors if some errors happened while saving user.
		if ( ! $po->mwpw_user->mangopay_id ) {

			$str_errors = mwpw_errors::mwpw_errors_get( 'users' );
			$str_errors .= mwpw_errors::mwpw_errors_get( 'gateway' );

			edd_set_error('verify user', $str_errors , 'mwpw');

			mwpw_errors::mwpw_errors_clean( 'users');
			mwpw_errors::mwpw_errors_clean( 'gateway');

			$error = true;

		}

		//Payment will be done towards site wallet...
		// ... in the lister function mwpw_listen_for_mangopayweb_ipn() we will transfer
		// corresponding amounts to each wallet
		if ( !$error ) {

			// set a listener for MangoPay validator Card callback
			$listener_url = trailingslashit( home_url( 'index.php' ) ) . '?edd-listener=mangopaywebIPN';

			// get url redirection through MangoPay\PayIn object
			$merchant_url = $po->mwpw_payIn( $purchase_data['price'], $purchase_data['fees'], $listener_url, $payment );

			// redirect to server
			if ( $merchant_url ) {
				wp_redirect ( $merchant_url );
				die;
			} else {
				edd_set_error( 'Gateway', mwpw_errors::mwpw_errors_get( 'gateway' ) );
				mwpw_errors::mwpw_errors_clean( 'gateway' );
			}

		}

	}

	// if errors are present, send the user back to the purchase page so they can be corrected
	edd_send_back_to_checkout( '?payment-mode=' . $purchase_data['post_data']['edd-gateway'] );

}
add_action( 'edd_gateway_mangopayweb_gateway', 'mwpw\mwpw_process_payment' );


/**
 *
 * Listens for EDDlistener with $_GET['mangopaywebIPN']
 * Requires $_GET['transactionId'] from IPN related to a PayIn
 * that brings in TAG field an integer meaning EDD payment that has been processed.
 * As payment has been done to site wallet, will finalize payment and redistribute funds to related wallets.
 */
function mwpw_listen_for_mangopayweb_ipn() {

	//Regular Mangopay IPN
	if ( isset( $_GET['edd-listener'] ) && $_GET['edd-listener'] == 'mangopaywebIPN' ) {

		// retrieve PayIn id from get
		$m_transaction_id =  $_GET["transactionId"];

		// fetch PayIn object
		require_once ( __DIR__ . "/includes/mwpw_pay.inc");
		$po = new mwpw_pay( wp_get_current_user() );
		$createdPayIn = $po->mwpw_fetch_payin( $m_transaction_id );

		if ( $createdPayIn )
			$Succeeded = $createdPayIn->Status == 'SUCCEEDED';

		if ( $Succeeded )
			$edd_payment_id = $createdPayIn->Tag;

		// if paying was done and not already processed purchase, distribute fundraised amount
		if ( $Succeeded && ( get_post( $edd_payment_id )->post_status != 'publish' ) ) {

			$po->mwpw_distribute_funds( $edd_payment_id );

			// update EDD payment history
			edd_update_payment_status( $edd_payment_id, 'complete' );

			// save any ocurred errors while distributing
			edd_insert_payment_note( $createdPayIn->Tag, mwpw_errors::mwpw_errors_get( 'IPN' ) );

            		// go to the success page
			edd_send_to_success_page();

		} else {
			wp_redirect( network_site_url( '/' ) ) ;
		}
	}

}
add_action( 'init', 'mwpw\mwpw_listen_for_mangopayweb_ipn' );
