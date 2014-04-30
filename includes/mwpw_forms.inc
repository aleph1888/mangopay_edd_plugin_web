<?php

/**
 * forms
 *
 * Provides methods to display MangoPay\User and Bank input forms.
 *
 * @package		mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 * @uses	mwpw_print.inc
 */

namespace mwpw;

require_once (  __DIR__ . '/mwpw_print.inc' );


/**
 *
 * Add needed js library
 */
function mwpw_enqueue_jquery() {
	wp_register_script ( "mwpw_js", plugins_url( 'mwpw.js', __FILE__ ), array( 'jquery' ) );
	wp_enqueue_script( 'mwpw_js' );
}

if ( is_admin() )
	add_action( 'admin_enqueue_scripts', 'mwpw\mwpw_enqueue_jquery' );
else
	add_action( 'wp_enqueue_scripts', 'mwpw\mwpw_enqueue_jquery' );


class mwpw_forms {

	/**
	*
	* Echoes a DIV block with a checkbox to toggle between UserNatural and UserLegal forms.
	*
	* @params mwpw_user @defaults
	* @params bool $display style.visibility
	*
	* @uses mwpw_print
	*/
	static private function mwpw_user_section_header( $defaults, $display = true ) {

		return mwpw_print_div ( true, 'mangopay_userheader', $display ) .
			mwpw_print_tag ( "h3", 'mangopay_user_info' ) .
			mwpw_print_table ( true ) .
			mwpw_print_tag ( "label", __( 'MangoPay\User Id: ', 'mwpw' ) . ( $defaults->mangopay_id ? $defaults->mangopay_id : ' -' ) ) .
				mwpw_print_checkbox ( 'mwpw_user_type', $defaults->is_legal_user) .
			mwpw_print_table ( false ) .
			mwpw_print_div ( false);

	}

	/**
	 *
	 * Returns echoable TABLE with two columns and one TR for every field representing MangoPay\user input data form.
	 * Will include a checkbox to toggle between UserNatural and UserLegal.
	 * Will include a notification for JavaScript file to hide if found as expected.
	 *
	 * @params mwpw_user $defaults
	 * @params bool $display style.visibility If FALSE a button to toggle visibility will be shown
	 *
	 * @uses mwpw_fields.inc\mwpw_get_fields()
	 * @uses mwpw_print.inc\mwpw_print_button()
	 * @uses mwpw_print.inc\mwpw_print_section()
	 *
	 * @return string
	 */
	static function mwpw_show_user_section( $defaults, $display = true ) {

		//Print a DIV ERROR that JS library will hide if everything is fine. Just for sandboxing.
		$output .= mwpw_print_div ( true, 'debubinfo', 'true' );
		$output .= mwpw_print_tag ( "label", "<br>\n --------------------------------------------------" );
		$output .= mwpw_print_tag ( "label", "JQUERY library missing" );
		$output .= mwpw_print_tag ( "label", "-------------------------------------------------- <br>\n" );
		$output .= mwpw_print_div ( false);

		//Show button to toggle visibility if need
		if ( ! $display )
			$output .= mwpw_print_button( array('mwpw_change_user_data') );

		//Header
		$output .= self::mwpw_user_section_header( $defaults, $display );

		//Will display this block if is user legal
		$display_block = $defaults->is_legal_user && $display;
		$yFields = mwpw_get_fields( 'Legal' );
		$output .= mwpw_print_section( "mangopay_legal", $defaults, $yFields, $display_block );

		//Will display this block if is user natural
		$display_block = ( ! $defaults->is_legal_user ) && $display;
		$yFields = mwpw_get_fields ( 'Natural' );
		$output .= mwpw_print_section( "mangopay_natural", $defaults, $yFields, $display_block );

		return $output;

	}

	/**
	 *
	 * Returns echoable TABLE with two columns and one TR for every field representing MangoPay\bankAccount input data form.
	 *
	 * @params mwpw_bank $defaults
	 *
	 * @uses mwpw_fields.inc\mwpw_get_fields()
	 * @uses mwpw_print.inc\mwpw_print_section()
	 *
	 * @return string
	 */
	static function mwpw_show_bank_section ( $defaults ) {

		$yFields = mwpw_get_fields( 'Bank' );
		$output = mwpw_print_tag ( "label", __( 'MangoPay\Bank Id: ', 'mwpw' ) . ( $defaults->bank_id ? $defaults->bank_id : ' -' ) ) . 
		$output .= mwpw_print_section( "mangopay_bank", $defaults, $yFields, true );

		return $output;

	}


	/**
	 *
	 * Returns echoable TABLE with two columns and one TR for every field representing MangoPay\bankAccount input data readonly form.
	 *
	 * @params mwpw_bank $defaults
	 *
	 * @uses mwpw_fields.inc\mwpw_get_fields()
	 * @uses mwpw_print.inc\mwpw_print_section()
	 *
	 * @return string
	 */
	static function mwpw_show_bank_section_info( $defaults ) {

		$yFields = mwpw_get_fields ( 'Bank' );
		$display = true; $desc = false;
		return mwpw_print_section_info("mangopay_bank", $defaults, $yFields, $display, null, $desc );

	}


	/**
	 *
	 * Shows Login and Register links, if need
	 *
	 * @return string
	 */
	static function mwpw_show_wordpress_login() {

		if ( ! is_user_logged_in() ) {
			return	mwpw_print_link ( __("Login", 'mwpw'), wp_login_url() ) . "<br>" .
				mwpw_print_link ( __("Register", 'mwpw'), wp_registration_url() );
		}

	}

}

?>
