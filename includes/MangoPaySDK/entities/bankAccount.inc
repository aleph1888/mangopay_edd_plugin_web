<?php
namespace MangoPay;

/**
 * Bank Account entity
 */
class BankAccount extends EntityBase {
    
    /**
     * User identifier
     * @var LeetchiId 
     */
    public $UserId;
    
    /**
     * Type of bank account
     * @var string 
     */
    public $Type;
    
    /**
     * Owner name
     * @var string 
     */
    public $OwnerName;
    
    /**
     * Owner address
     * @var string 
     */
    public $OwnerAddress;
    
    /**
     * IBAN number 
     * @var string 
     */
    public $IBAN;
    
    /**
     * BIC
     * @var string 
     */
    public $BIC;
    
    /**
     * Get array with read-only properties
     * @return array
     */
    public function GetReadOnlyProperties() {
        $properties = parent::GetReadOnlyProperties();
        array_push( $properties, 'UserId' );
        
        return $properties;
    }
}
