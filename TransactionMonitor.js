const Web3 = require('web3');
const web3 = new Web3('https://mainnet.infura.io/v3/YOUR_INFURA_PROJECT_ID');

const address = '0xYourAddressHere';

web3.eth.subscribe('pendingTransactions', (error, txHash) => {
  if (error) console.error(error);
  else {
    web3.eth.getTransaction(txHash, (err, tx) => {
      if (tx && tx.to === address) {
        console.log('Incoming transaction detected:', tx);
      }
    });
  }
});
