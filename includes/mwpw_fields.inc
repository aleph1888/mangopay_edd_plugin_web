<?php

/**
 *
 * Provides some auxiliar functions related to Mangopay fields management.
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

/**
 *
 * This is for automatized form generation.
 *
 * Returns array of strings containing MangoPay\{User or BankAccount} minimum required fields.
 *
 * Add fields here to be displayed on forms and saved in BBDD, any field must exists in MangoPaySDK.
 *
 * Also provide localization in proper languages/mwpwXXXX.po file, noting that every field
 * should be together with a {msgid = "fieldname_description"} translation. Set space if you don't need it.
 *
 * @params string $str_type a value in ( 'Natural', 'Legal', 'Bank' )
 *
 * @return array
 */
function mwpw_get_fields( $str_type ) {

	switch ( $str_type ) {
		case "Natural":
			$yFields =  array( "Email", "FirstName", "LastName", "Birthday", "Nationality", "CountryOfResidence" );
			break;

		case "Legal":
			//Notice: LegalEmail field, that does not correspond with Mangopay API.
			//Will need to manage manually across the flow.
			//Conflict in form with Natural.Email field on Crowfunding.checkout Personal Information form.
			$yFields = array( "LegalEmail", "Name", "LegalPersonType", "LegalRepresentativeFirstName", "LegalRepresentativeLastName",
				"LegalRepresentativeBirthday", "LegalRepresentativeNationality", "LegalRepresentativeCountryOfResidence");
			break;

		case "Bank":
			$yFields = array( "OwnerName", "OwnerAddress", "IBAN", "BIC" );
	}

	return $yFields;

}

/**
 *
 * Return true or false wether any of the specified fields has changed between $values and $entity
 *
 * @params array $yFields [see mwpw_get_fields]
 * @params object $entity  All fields presents in $yFields will be expected in this object $entity
 * @params array or object $values Function will manage an object(like mwpw_user or mwpw_bank) or an array(like $_POST).
 *				   Notice: If is_array, then will expect keys prefixed by 'mwpw' literal
 *
 * @return bool
 */
function mwpw_has_changed_fields( $yFields, $entity, $values ) {

	if ( is_array( $values ) ) {

		foreach ( $yFields as $field ) {
			$has_changed = ( $values["mwpw_{$field}"] && ( $values["mwpw_{$field}"] != $entity->$field ) );
			if ( $has_changed )
				return true;

		}
	}
	else {

		foreach ( $yFields as $field ) {
			$has_changed = ( $values->$field && ( $values->$field != $entity->$field ) );
			if ( $has_changed )
				return true;
		}

	}

	return false;
}

?>
