<?php
namespace MangoPay;

/**
 * Class to management MangoPay API for cards
 */
class ApiCards extends ApiBase {
    
    /**
     * Get card
     * @param int $cardId Card identifier
     * @return \MangoPay\Card object returned from API
     */
    public function Get($cardId) {
        return $this->GetObject('card_get', $cardId, '\MangoPay\Card');
    }
}