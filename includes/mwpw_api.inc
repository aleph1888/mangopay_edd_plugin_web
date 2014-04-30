<?php

/**
 * Api
 *
 * Singleton class for plugin to access MangoPaySDKv2 extending MangoPay\MangoPayApi.
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

use MangoPay as m;

include 'MangoPaySDK/mangoPayApi.inc';
 

/**
 *
 *	Expected section defined global vars. [See mwpw_gateway.php/mwpw_init_site()]:
 *		define('MWP_client_id', '<client_id>');
 *		define('MWP_password', '<pass>');
 *		define('MWP_temp_path', '<path to temp folder where MangoPay API will write temp info>');
 *		define('MWP_base_path', 'https://api.sandbox.mangopay.com' or 'https://api.mangopay.com');
 *	[notice that MWP_base_path will need to be setted also in MangoPaySDK/types/configuration.inc (search for public $BaseUrl)]
 */
class mwpw_api extends m\MangoPayApi {

	static private $instance;

	public static function get_instance() {

		if ( self::$instance == null ) {

			self::$instance = new mwpw_api;

			self::$instance->Config->ClientId = MWPW_client_id;
			self::$instance->Config->ClientPassword = MWPW_password;
			self::$instance->Config->TemporaryFolder = MWPW_temp_path;
			self::$instance->Config->BaseUrl = MWPW_base_path;

		}

		return self::$instance;

	}

}




