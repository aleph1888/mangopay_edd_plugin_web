<?php

/**
 * Errors
 *
 * This class is for manage errors through EDD_Session class
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 *
 * @uses	EDD()->session [All errors keys will be prefixed by 'mwpw' literal]
 */

namespace mwpw;


class mwpw_errors {

	/**
	 * Process an Mangopay\Exception object by adding errors to stack reading details.
	 *
	 * @param string key Will use this key to get errors
	 * @param Mangopay\ResponseException $e Error object
	 * @param string $source Any user-friendly literal to inform the error source
	 * @param bool $append Append the error to stack, otherwise add the message erasing previous errors.
	 */
	static public function mwpw_errors_manage_api( $str_key, $e, $source, $append = true ) {

		$output = "";

		if ( $e->GetErrorDetails()->Errors ) {
			foreach ( $e->GetErrorDetails()->Errors as $error )
				$output .= $error . "\n";
		}

		if ( $append ) {
			self::mwpw_errors_append( $str_key, "{$source}: {$e->GetMessage()} {$output}<br>" );
		} else {
			self::mwpw_errors_add( $str_key, "{$source}: {$e->GetMessage()} {$output}<br>" );
		}

	}


	/**
	 * Manage Exception object
	 *
	 * @params string $str_key Prefix for error in stack.
	 * @param MangoPay\Exception $e Error object
	 * @param string $source Any literal to inform the error source
	 */
	static public function mwpw_errors_manage( $str_key, $e, $source ) {

		self::mwpw_errors_append( $str_key, "{$source}: {$e->message}\n" );

	}


	/**
	 *
	 * Clears errors stack.
	 *
	 * @params string $str_key Prefix for error in stack.
	 */
	static public function mwpw_errors_clean( $str_key ) {

		mwpw_errors::mwpw_errors_add( $str_key , "" );

	}


	/**
	 * Appends a new error to errors stack
	 *
	 * @params string $str_key Prefix for error in stack.
	 * @param string $str_error
	 *
	 * @uses EDD_Session::set()
	 */
	static public function mwpw_errors_append( $str_key, $str_error ) {

		$str_error = EDD()->session->get( 'mwpw_' . $str_key ) . "<br>" . $str_error;
		EDD()->session->set( 'mwpw_' . $str_key, $str_error );

	}


	/**
	 * Adds a new error to errors stack, erasing all previous errors.
	 *
	 * @param string $str_key Prefix for error in stack
	 * @param string $str_error
	 *
	 * @uses EDD_Session::set()
	 */
	static public function mwpw_errors_add( $str_key, $str_error ) {

		EDD()->session->set( 'mwpw_' . $str_key , $str_error );

	}

	/**
	 *
	 * Get all errors from errors stack.
	 *
	 *
	 * @params string $str_key Prefix for error in stack.
	 *
	 * @return string
	 *
	 * @uses EDD_Session::get()
	 */
	static public function mwpw_errors_get( $str_key ) {

		return EDD()->session->get( 'mwpw_' . $str_key );

	}

	/**
	 *
	 * Returns nice <div> block with errors stored on stack.
	 *
	 * @params string $str_key Prefix for error in stack.
	 *
	 * @return string
	 */
	static public function mwpw_errors_print( $str_key ) {

		echo "<br><div class='message'>" .
				htmlspecialchars( self::mwpw_errors_get( $str_key ) ) .
		"</div>";

	}

	/**
	 *
	 * Will process a 'not found id' error by providing a listener link
	 * where erasing id is performed
	 *
	 * @params MangoPay\ResponseException
	 * @params integer $entity_id
	 * @params bool $report_missing
	 * @params string $listener_url
	 * @params string $context_error Key for retrieve error
	 * @params string $context_string User friendly error localization
	 *
	 * @uses mwpw_print_link
	 */
	static public function mwpw_errors_entity_not_found( 	$e,
								$entity_id,
								$report_missing,
								$listener_url,
								$str_key,
								$context_string ) {

		if ( $e->GetErrorDetails()->Message == "The ressource does not exist" ) {

				if ( $report_missing ) {
					mwpw_errors::mwpw_errors_manage_api ( $str_key, $e, $context_string );
					$str_button_caption = __( "Delete {$entity_id} id so a new one can be setted", 'mwpw' );
					$button = mwpw_print_link ( $str_button_caption, $str_listerner_url );
					mwpw_errors::mwpw_errors_append ( $str_key, $button );
				}

		} else {

			mwpw_errors::mwpw_errors_manage_api ( $context_error, $e, __( 'Fetching wallet', 'mwpw') );

		}

	}

}

?>
