/**
 * This is to work with mwpw_forms on static method mwpw_show_user_section(),
 * so it will be loaded in user profile and in gateway cc form.
 *
 * a) Main goal is to toogle between MangoPay\User legal and natural forms.
 * b) Manages hidding crowfunding personal data div, in order to use our own.
 * c) Manages showing form for loggedin users that wants to change data.
 * d) Hides a JavaScript missing notification
 *
 * BUG: Althoug in wp-admin|profile file is loaded and is working,
 *	in cc form is loaded but not working, (even in firebug is working!!!)
 *
 * @package	mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 * @uses	includes/mwpw_form.inc
 */

jQuery(document).ready(function() {

	/**
	*
	* Form checkbox to toogle user forms
	*/
	function change_user_display() {
		if ( jQuery('.mwpw_user_type').is(":checked") ) {
			jQuery('.mangopay_legal').show();
			jQuery('.mangopay_natural').hide();
		} else {
			jQuery('.mangopay_natural').show();
			jQuery('.mangopay_legal').hide();
		}
	}
	jQuery('.mwpw_user_type').click(function() {
		change_user_display ();
	});


	/**
	*
	* Payform button to change section already filled.
	*/
	jQuery('.mwpw_button_change_user_data').click(function() {
		jQuery('.mangopay_userheader').show();
		change_user_display ();
		jQuery('.bt_change_user_data').hide();
	});


	/**
	*
	* General jquery object hidder
	*/
	jQuery.fn.exists = function(){return this.length>0;}
	function hide_object ( object ) {
		if ( object.exists() ) {
			object.hide();
		}
	}


	/**
	*
	* Will will Crowfunding plugin Personal Info fieldset
	* with our plugin fields, so set values and hide...
	*/
	hide_object ( jQuery("#edd_checkout_user_info") );
	hide_object ( jQuery("#edd-login-account-wrap") );
	hide_object ( jQuery("#edd-user-first-name-wrap") );
	hide_object ( jQuery("#edd-user-last-name-wrap") );
	hide_object ( jQuery("#edd-user-email-wrap") );

	jQuery("#mwp_Email").change(function() {
		jQuery('#edd-email').val(jQuery("#mwp_Email").val());
	});
	jQuery("#mwp_FirstName").change(function() {
		jQuery('#edd-first').val(jQuery("#mwp_FirstName").val());
	});
	jQuery("#mwp_LastName").change(function() {
		jQuery('#edd-last').val(jQuery("#mwp_LastName").val());
	});
	jQuery("#mwp_LegalEmail").change(function() {
		jQuery('#edd-email').val(jQuery("#mwp_LegalEmail").val());
	});
	jQuery("#mwp_LegalRepresentativeFirstName").change(function() {
		jQuery('#edd-first').val(jQuery("#mwp_LegalRepresentativeFirstName").val());
	});
	jQuery("#mwp_LegalRepresentativeLastName").change(function() {
		jQuery('#edd-last').val(jQuery("#mwp_LegalRepresentativeLastName").val());
	});


	/**
	*
	* Missing javascript. Will hardcode a notice that this function will hide.
	*/
	jQuery('.debubinfo').hide();

});
