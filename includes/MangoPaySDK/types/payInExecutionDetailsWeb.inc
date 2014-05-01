<?php
namespace MangoPay;

/**
 * Class represents Web type for execution option in PayIn entity
 */
class PayInExecutionDetailsWeb extends Dto implements PayInExecutionDetails {

    /**
     * URL format expected
     * @var string
     */
    public $RedirectURL;
    
    /**
     * URL format expected
     * @var string
     */
    public $ReturnURL;
    
    /**
     * URL format expected.
     * @var string
     */
    public $TemplateURL;
    
    /**
     * @var string
     */
    public $Culture;
    
    /**
     * Mode3DSType { DEFAULT, FORCE }
     * @var string
     */
    public $SecureMode;
    
    /**
     * Get array with read-only properties
     * @return array
     */
    public function GetReadOnlyProperties() {
        $properties = parent::GetReadOnlyProperties();
        array_push( $properties, 'RedirectURL' );
        
        return $properties;
    }
}