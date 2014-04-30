<?php

/**
 * Profile
 *
 * This file is for manage User and Bank forms on profile section.
 * Adds Mangopay user section at the end of user profile to create Mangopay/User and Mangopay/Bank due
 * Download_Post owner needs to create a Mangopay/BankAccount object in order to withdraw money store on wallet.
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;


/**
 *
 * Show messages on profile box by catching 'Profile updated' &  'User updated.' translations.
 */
function mwpw_display_errors ($translated_text, $untranslated_text, $text){

	$str_errors = mwpw_errors::mwpw_errors_get( 'banks' );
	$str_errors .= mwpw_errors::mwpw_errors_get( 'users' );

	if ( $str_error && ( $translated_text == 'Profile updated.' ||  $translated_text == 'User updated.') ) {

		mwpw_errors::mwpw_errors_clean( 'banks' );
		mwpw_errors::mwpw_errors_clean( 'users' );

		return __ ( 'save_profile_error', 'mwpw' ) . '<br>' . $str_errors ;
	}

	return $translated_text;

}
add_filter( 'gettext', 'mwpw\mwpw_display_errors', 10, 3 );


/**
 *
 * Echoes section, whenever a user profile is required.
 *
 */
function mwpw_show_profile_forms() {

	//Get editing user from $_GET or current loggedin user
	$wp_user_id = $_GET['user_id'];
	if ( $wp_user_id ) {
		$wp_user = get_user_by('id', $wp_user_id);
	}
	if ( ! $wp_user ) {
		$wp_user = wp_get_current_user();
	}

	//Construct mwpw_user
	$autosave = false;
	$str_listener_url = trailingslashit( home_url( 'index.php' ) ) . '?edd-listener=mangopaywebUserErase';
	$str_listener_url .= "&u={$wp_user->ID}";
	$mwpw_user = new mwpw_user( $wp_user, $autosave, $str_listener_url );
	echo mwpw_forms::mwpw_show_user_section( $mwpw_user );

	//Show form bank, if needed
	if ( mwpw_user::mwpw_is_owner( $wp_user ) ) {
		$str_listener_url = trailingslashit( home_url( 'index.php' ) ) . '?edd-listener=mangopaywebBankErase';
		$str_listener_url .= "&u={$wp_user->ID}";
		$mwpw_bank = new mwpw_bank ( $mwpw_user, $str_listener_url );
		echo mwpw_forms::mwpw_show_bank_section( $mwpw_bank );
	}

	//Show errors
	mwpw_admin_notices_profile();

}
add_action( 'show_user_profile', 'mwpw\mwpw_show_profile_forms' );
add_action( 'edit_user_profile', 'mwpw\mwpw_show_profile_forms' );


/**
 *
 * Manage saving MangoPay's user and bank account, whenever a user profile is required.
 */
function mwpw_save_profile_forms() {

        //Get editing user from post or current user
        $wp_user_id = $_POST['user_id'];
        if ( $wp_user_id ) {
                $wp_user = get_user_by ('id', $wp_user_id);
        }
        if ( ! $wp_user ) {
                $wp_user = wp_get_current_user();
        }

        //Gatekeeper
	if ( ! current_user_can( 'edit_user', $wp_user->ID ) )
		return false;

	//Saves user from $_POST data
	$m_user_id = mwpw_user::mwpw_save_user( $wp_user, $new_bank_id );

	//Updates WP_user ids. Notices that if has changed user type from Legal to Natural
	//or viceversa will create a new BankAccount as MangoApi sdk2 CRUD limitation
	if ( $m_user_id && $wp_user-mangopay_id != $m_user_id ) {

		update_user_meta( $wp_user_id, 'mangopay_id', $m_user_id );

		if ( $new_bank_id )
			update_user_meta( $wp_user_id, 'bank_id', $new_bank_id );

	}

	//Save mangopay bankaccount with POST data
	$wp_user = get_user_by ('id', $wp_user_id); //Refetch if a new bank id has saved due a user type changing
	$m_bank_id = mwpw_bank::mwpw_save_bank( $wp_user );

	if ( $m_bank_id && $wp_user->bank_id != $m_bank_id )
		update_user_meta( $wp_user_id, 'bank_id', $m_bank_id );

}
add_action( 'personal_options_update', 'mwpw\mwpw_save_profile_forms' );
add_action( 'edit_user_profile_update', 'mwpw\mwpw_save_profile_forms' );


/**
 *
 * Reads errors for key 'profile' and display any existing one for admin users, after cleans stack.
 * Called in mwpw_show_profile_forms()
 *
 * @uses mwpw_errors class
 * @uses add_settings_error, settings_errors
 */
function mwpw_admin_notices_profile() {

	//Print any init errors
	$str_errors = mwpw_errors::mwpw_errors_get( 'users' );
	$str_errors .= mwpw_errors::mwpw_errors_get( 'banks' );

	if (  $str_errors ) {

		$plugin_name = __( 'Easy Digital Downloads - Mangopay Gateway through Web: ', 'mwpw' ) . "<br>";

		add_settings_error( 'mwpwprofile', '', $plugin_name . $str_errors , 'error' );
		mwpw_errors::mwpw_errors_clean( 'banks' );
		mwpw_errors::mwpw_errors_clean( 'users' );

		settings_errors( 'mwpwprofile', false ) ;
	}

}
