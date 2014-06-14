<?php

/**
 * Post
 *
 * This file is for manage Bank withdraw submit on post edit sidebar section.
 * Once the post autor has configured bank data on his profile, withdraw can be requested.
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

use Mangopay as m;

/**
 *
 * Prints payout info for a post wallet:
 * z) Pending payout info (then will break until it is commited by MangoPay staff).
 * a) Total amount stored.
 * b) BankAccount withdraw info. (Fields configured on author's profile.)
 * c) Payout request button.
 */
function mwpw_print_meta_box( $wp_post ) {

	//Object creation
	require_once __DIR__ . "/includes/mwpw_payout.inc";
	$m_payout = mwpw_payout::mwpw_get_payout( $wp_post->payout_id );

	// notices of pending payout
	if ( $m_payout->Status == 'CREATED' ) {
		echo __( 'You have a pending payout. MangoPay staff will transfer funds to your bank account', 'mwpw' );
		return;
	} elseif ( $m_payout->Status == 'SUCCEEDED' ) {
		$str_done_payout = __( 'Collected funds has been transfered to your bank account.', 'mwpw' );
		update_post_meta( $wp_post->ID, 'payout_id', 0 );
	}

	// load wallet
	$m_wallet_id = $wp_post->wallet_id;
	$owner_id = $wp_post->ID;
	$owner_type = 'post';
	$context_error = "payout";
	$mwpw_wallet = new mwpw_wallet( $m_wallet_id, $owner_id, $owner_type, $context_error );
	$create = FALSE;
	$mwpw_wallet->mwpw_load_wallet( $create );

	// echoes total section
	if ( $mwpw_wallet )
		echo  "<h2>" . __('Total: ', 'mwpw') . $mwpw_wallet->m_wallet->Balance->Amount / 100 . __( ' eur', 'mwpw') . "</h2>\n";

	//Print Bankaccount info and button
	echo mwpw_payout::mwpw_bankaccount_get_info( $wp_post );

	//Show errors
	mwpw_admin_notices_post();

}


/**
 * Calls for metabox
 *
 * @params WP_post $wp_post
 */
function mwpw_show_post_fields( $wp_post ) {

	//Gatekeeper
	$wp_user = get_userdata( $wp_post->post_author );
	if ( wp_get_current_user() != $user )
		//Print box
		add_meta_box( $post->ID, __( "post_fields_title", 'mwpw'), "mwpw\mwpw_print_meta_box", 'download', 'side', 'high');

}
add_action( 'add_meta_boxes', 'mwpw\mwpw_show_post_fields' );


/**
 *
 * Reads errors for key 'payout' and display any existing for admin users, after cleans stack,
 * Calle in mwpw_print_meta_box()
 *
 * @uses mwpw_errors class
 */
function mwpw_admin_notices_post() {

	echo "<br>" . mwpw_errors::mwpw_errors_get( 'payout' );
	mwpw_errors::mwpw_errors_clean( 'payout' );

}
