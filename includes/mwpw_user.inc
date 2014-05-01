<?php

/**
 * User
 *
 * Extends MangoPay\User class and exposes some statics methods to manage users.
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

use MangoPay as m;

class mwpw_user extends m\User {

	public $ob_id;			//@param integer Site user id
	public $mangopay_id; 		//@param integer Mangopay user id
	public $bank_id;		//@param integer Mangopay bank id

	public $is_legal_user;		//@param bool Wether UserLegal or UserNatural
	public $LegalEmail;		//@param string As MangoPay\UserLegal field is called Email instead LegalEmail, wrap it.

	/**
	*
	* Fetch MangoPay\user associated with $ob_user or creates a new one (if required).
	*
	* @params object $ob_user An object with (at least) three fields: ID, mangopay_id and bank_id
	* @params bool $autosave Creates a new MangoPay\User from $_POST data.
	* @params string $str_listerner_url If bad usr_id found, will redirect to this listener in order to erase it.
	* @params integer by Ref $new_bank_id Will be setted if a new user was created due a type change and had a bankaccount associated.
	* @params bool $report_missing Controls invalid MangoPay id notification erase link.
	*/
	public function __construct ( 	$ob_user,
					$autosave = FALSE,
					$str_listerner_url = NULL,
					&$new_bank_id = 0,
					$report_missing = TRUE ) {

		if ( $ob_user )
			$this->ob_id = $ob_user->ID;

		if ( $ob_user->mangopay_id  )
			$this->mangopay_id = $ob_user->mangopay_id;

		if ( $ob_user->bank_id )
			$this->bank_id = $ob_user->bank_id;

		if ( $autosave )
			$this->mangopay_id =  self::mwpw_save( $ob_user, $this, $new_bank_id );

		try {

			if ( $this->mangopay_id )
				$m_user = mwpw_api::get_instance()->Users->Get( $this->mangopay_id );

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_entity_not_found(
									$e,
									$this->mangopay_id,
									$report_missing,
									$listener_url,
									'users',
									__( 'Getting user', 'mwpw')
								);

		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage ( 'users', $e, __( 'Getting user', 'mwpw') );
		}

		if ( $m_user )
			$this->mwpw_set_values( $m_user );
		else
			$this->mwpw_default_values( $ob_user );

	}


	/**
	*
	* Fills this class with default values from param object and from site.
	*
	* @params object $ob_user
	*/
	private function mwpw_default_values( $ob_user = null ) {

		// to generaliza this could be:
		// if ( $ob_user && get_class ( $ob_user ) == 'WP_user' )
		if ( $ob_user ) {
			$this->Email = $ob_user->user_email;
			$this->FirstName = $ob_user->user_firstname;
			$this->LastName = $ob_user->user_lastname;
		}

		// determine site locale
		$iso_code = substr( get_locale(),  3, 2 );

		// natural user
		$this->Nationality = $iso_code;
		$this->CountryOfResidence = $iso_code;

		// legal user
		$this->LegalRepresentativeNationality = $iso_code;
		$this->LegalRepresentativeCountryOfResidence = $iso_code;
		$this->LegalRepresentativeNationality = $iso_code;
		$this->LegalRepresentativeCountryOfResidence = $iso_code;

	}


	/**
	*
	* Fills this class with data from param by getting expected fields list
	*
	* @params mwpw_user $m_user
	*
	* @uses mwpw_fields.inc\mwpw_get_fields()
	*/
	private function mwpw_set_values( $m_user ) {

		$this->is_legal_user = get_class( $m_user ) == 'MangoPay\UserLegal';

		$yFields = mwpw_get_fields( ( $this->is_legal_user ? 'Legal' : 'Natural') );

		foreach ( $yFields as $field )
			$this->$field = $m_user->$field;

		if ( $this->is_legal_user )
			$this->LegalEmail = $m_user->Email;

		self::mwpw_convert_number_to_dates( $this );

	}


	/**
	*
	* Static method to save users from $_POST data; fields are expected to be prefixed by 'mwpw_'
	*
	* @param object $ob_user with at least one field: mangopay_id field.
	*
	* @return integer MangoPay\User (Legal or Natural) identificator
	*
	* @uses mwpw_fields.inc\mwpw_get_fields()
	* @uses mwpw_fields.inc\mwpw_has_changed_fields()
	*/
	static public function mwpw_save( $ob_user, $mwpw_user, &$new_bank_id = 0 ) {

		$api = mwpw_api::get_instance();

		//Is legal or natural user
		$is_legal_user = $_POST["mwpw_user_type"] == "on";

		//Notice: if user type changes we need to create a new user
		$has_changed_type = $mwpw_user->is_legal_user != $is_legal_user;

		//Get fields names switching on user type
		$yFields = mwpw_get_fields( ( $is_legal_user ?  'Legal' : 'Natural' ) );

		if ( ! $has_changed_type && ! mwpw_has_changed_fields( $yFields, $mwpw_user, $_POST )  )
			return $ob_user->mangopay_id;

		//Instantitate a new MangoPay user object
		$m_user = ( $is_legal_user ?  new m\UserLegal() : new m\UserNatural() );

		//Fill data
		foreach ( $yFields as $field )
			$m_user->$field = $_POST["mwpw_{$field}"];

		if ( $is_legal_user )
			$m_user->Email = $_POST["mwpw_LegalEmail"]; //this rare case due conflict with Crowfunding plugin.

		self::mwpw_convert_dates_to_number( $m_user );

		try {

			//Execute saving
			if ( $ob_user->mangopay_id && ! $has_changed_type ) {
				$m_user->Id = $ob_user->mangopay_id;
				$m_user = $api->Users->Update( $m_user ) ;
			} else {
				$m_user = $api->Users->Create( $m_user );
				//Notice: If has changed type and had bankAccount we must reconfigurate bank
				if ( $has_changed_type && $ob_user->bank_id )
					$new_bank_id = mwpw_bank::mwpw_reset_bank_to_owner ( $ob_user, $m_user->Id );
			}

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api ( 'users', $e, __( 'User form', 'mwpw'), false );

		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage ( 'users', $e, __( 'User form', 'mwpw') );
		}

		return $m_user->Id;

	}


	/**
	*
	* Creates a new mwpw_user object based on param object and saves it. This is to habilitate $autosave in construct.
	*
	* @param object $ob_user with at least two fields: mangopay_id field.
	* @params integer by Ref $new_bank_id Will be setted if a new user was created due a type change and had a bankaccount associated.
	*
	* @return integer MangoPay\User (Legal or Natural) identificator
	*/
	static public function mwpw_save_user( $ob_user, &$new_bank_id = 0 ) {

		$mwpw_user = new mwpw_user( $ob_user );

		return self:: mwpw_save($ob_user, $mwpw_user, $new_bank_id );
	}


	/**
	*
	* Parse birthday (legal or natuaral) date to strtotime() seconds from Unix Epoch, depending of param subtype.
	*
	* @params object by Ref $user with at least three fields: if is_legal_user then LegalRepresentativeBirthday, else Birthday
	*/
	static function mwpw_convert_dates_to_number( &$m_user ) {

		if ( get_class( $m_user ) == 'MangoPay\UserLegal' ) {
			$m_user->LegalRepresentativeBirthday = strtotime( $m_user->LegalRepresentativeBirthday );
		} else {
			$m_user->Birthday = strtotime ( $m_user->Birthday );
		}


	}


	/**
	*
	* Parse birthday (legal or natural) strtotime() seconds from Unix Epoch to date depending of param subtype.
	*
	* @params object by Ref $user with at least three fields: if is_legal_user then LegalRepresentativeBirthday, else Birthday
	*/
	static function mwpw_convert_number_to_dates( &$m_user ) {

		if ( $user->is_legal_user ) {
			$user->LegalRepresentativeBirthday = date("Y-m-d", $user->LegalRepresentativeBirthday );
		} else {
			$user->Birthday = date("Y-m-d",  $user->Birthday );
		}

	}


	/**
	* Returns true if user role is owner
	*
	* @param WP_User $wp_user User to evaluate
	*/
	static public function mwpw_is_owner( $wp_user ){

		//return $user-> roles[0] != "suscriber" && $user-> roles[0] != "contributor";
		//TODO: Check this with Crowfunding user roles
		return true;

	}

}

?>
