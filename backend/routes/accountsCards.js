/* Tämä tiedosto on accounts_cards-liitostaulun reititys. Se kertoo Expressille, mihin URL-osoitteisiin vastataan ja mitä controller-funktiota
kutsutaan */

// Importit
const express = require('express');
const router = express.Router();

// Tuodaan controller-tiedoston funktiot käyttöön ja jokainen vastaa yhdestä toiminnosta liitostaululle
const {
  createLink,
  getAllLinks,
  getLinksByAccount,
  getLinksByCard,
  deleteLink,
} = require('../controllers/accountsCardsController');

// Reitit
router.post('/', createLink); // CREATE - luo kortti-tili linkin
router.get('/', getAllLinks); // READ - hakee kaikki linkit

router.get('/account/:idAccount', getLinksByAccount); // READ - hakee linkit tietylle tilille
router.get('/card/:idCard', getLinksByCard); // READ - hakee linkit tietylle kortille

router.delete('/', deleteLink); // DELETE - poistaa kortti-tili linkin

module.exports = router;
