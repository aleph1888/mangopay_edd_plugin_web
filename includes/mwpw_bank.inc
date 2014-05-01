<?php

/**
 * bank
 *
 * Extends MangoPay BankAccount class and exposes some statics methods to manage payouts.
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

use MangoPay as m;

class mwpw_bank extends m\BankAccount {

	/**
	*
	* Fetch MangoPay\BankAccount associated with $ob_user
	* Could happen a bad MangoPay Id is configured (ie: sandbox to production) A button to delete will be provided)
	*
	* @params object $ob_user An object with (at least) two fields: mangopay_id and bank_id.
	* @params string $str_listerner_url If bad bank_id found, will redirect to this listener in order to erase it.
	* @params bool $report_missing
	*
	* @uses mwpw_print.inc\mwpw_print_button()
	*/
	public function __construct( $ob_user, $str_listerner_url = NULL, $report_missing = TRUE ) {

		try {

			if ( $ob_user->mangopay_id && $ob_user->bank_id )
				$m_bank = mwpw_api::get_instance()->Users->GetBankAccount( $ob_user->mangopay_id, $ob_user->bank_id );

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_entity_not_found(
								$e,
								$this-bank_id,
								$report_missing,
								$listener_url,
								'banks',
								__( 'Getting bank', 'mwpw')
							);


		} catch ( m\Exception $e ) {

			mwpw_errors::mwpw_errors_manage ( 'banks', $e, __( 'Getting bank', 'mwpw') );

		}

		if ( $m_bank )
			$this->mwpw_set_values ( $m_bank );

		//Set ids
		$this->ob_id = $ob_user->ob_id;
		$this->mangopay_id = $ob_user->mangopay_id;
		$this->bank_id = $ob_user->bank_id;

	}

	/**
	 *
	 * Fills this class with data from param
	 *
	 * @params MangoPay\BankAccount $m_bank
	 *
	 * @uses mwpw_fields.inc\mwpw_get_fields()
	 */
	public function mwpw_set_values( $m_bank ) {

		if ( $m_bank ) {
			$yFields =  mwpw_get_fields( 'Bank' );
			foreach ( $yFields as $field )
				$this->$field = $m_bank->$field;
		}

	}

	/**
	*
	* Compares fields with $values fields to determinate if saving is need, and saves it.
	*
	* @params object $ob_user An object with (at least) two fields: mangopay_id and bank_id.
	* @params object or array $values If array will expect fields to be prefixed by 'mwpw_'
	* @params bool $report_missing
	*
	* @return integer MangoPay\BankAccount identificator
	*
	* @uses mwpw_fields.inc\mwpw_get_fields()
	* @uses mwpw_fields.inc\mwpw_has_changed_fields()
	*/
	static public function mwpw_save_bank_( $ob_user, $values, $report_missing = true ) {

		$bank = new mwpw_bank( $ob_user, null, $report_missing );

		// check for changes
		$yFields = mwpw_get_fields( 'Bank' );

		if ( ! mwpw_has_changed_fields( $yFields, $bank, $values ) )
			return $ob_user->bank_id;

		// cannot save if mangopay_id field is not present
		if ( ! $ob_user->mangopay_id ) {
			$Need_user_error = __( "You will be able to save bank once MangoPay user form is filled in profile", 'mwpw' );
			mwpw_errors::mwpw_errors_append ( 'banks', $Need_user_error );
			return $ob_user->bank_id;
		}

		// new object
		$bankAccount = new m\BankAccount();
		$bankAccount->UserId = $ob_user->mangopay_id;
		$bankAccount->Type = 'IBAN';

		// fill data
		if ( is_array( $values ) ) {
			foreach ( $yFields as $field )
				$bankAccount->$field = $values["mwpw_{$field}"];
		} else {
			foreach ( $yFields as $field )
				$bankAccount->$field = $values->$field;
		}

		try {
			$bankResult = mwpw_api::get_instance()->Users->CreateBankAccount( $ob_user->mangopay_id, $bankAccount );

			return $bankResult->Id;

		} catch ( m\ResponseException $e ) {

			mwpw_errors::mwpw_errors_manage_api ( 'banks', $e, __( 'Bank profile form', 'mwpw') );

		} catch ( m\Exception $e ) {
			mwpw_errors::mwpw_errors_manage ( 'banks', $e, __( 'Bank profile form', 'mwpw') );
		}

		return $mwpw_user->bank_id;
	}

	/**
	*
	* Calls mwpw_save_bank_ with values of $mwpw_bank or $_POST if null
	*
	* @params object $ob_user An object with (at least) two fields: mangopay_id and bank_id
	* @params object $mwpw_bank
	* @params bool $report_missing
	* @return integer MangoPay\BankAccount identificator
	*
	* @uses mwpw_fields.inc\mwpw_get_fields()
	* @uses mwpw_fields.inc\mwpw_has_changed_fields()
	*/
	static public function mwpw_save_bank( $ob_user, $mwpw_bank = NULL, $report_missing = TRUE ){

		if ( $mwpw_bank )
			return self::mwpw_save_bank_( $ob_user, $mwpw_bank, $report_missing );
		else
			return self::mwpw_save_bank_( $ob_user, $_POST, $report_missing );

	}


	/**
	 *
	 * Creates a new BankAccount object based on $ob_user assigning it to $new_user_id, and returns its id
	 *
	 * @params object $ob_user
	 * @params integer $new_user_id MangoPay\User
	 *
	 * @returns integer MangoPay\BankAccount identificator
	 */
	static public function mwpw_reset_bank_to_owner( $ob_user, $new_user_id ) {

		// fetch old bank
 		$mwpw_bank = new mwpw_bank( $ob_user, null, false );

 		// change id
 		$ob_user->mangopay_id = $new_user_id;

		// create new one and return id
		return self::mwpw_save_bank( $ob_user, $mwpw_bank, false );

	}
}

?>
