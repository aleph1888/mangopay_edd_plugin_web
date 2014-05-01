<?php
namespace MangoPay;

/**
 * Class represents Web type for execution option in PayIn entity
 */
class PayInExecutionDetailsDirect extends Dto implements PayInExecutionDetails {
    
    /**
     * SecureMode { DEFAULT, FORCE }
     * @var string 
     */
    public $SecureMode;
    
    /**
     * CardId
     * @var string 
     */
    public $CardId;
    
    /**
     * SecureModeReturnURL
     * @var string 
     */
    public $SecureModeReturnURL;
}