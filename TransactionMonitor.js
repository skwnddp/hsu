const Web3 = require('web3');
const web3 = new Web3('https://mainnet.infura.io/v3/YOUR_INFURA_PROJECT_ID');

const address = '0xYourAddressHere'.toLowerCase();

(async () => {
  const subscription = web3.eth.subscribe('pendingTransactions', async (error, txHash) => {
    if (error) {
      console.error('Error subscribing to pending transactions:', error);
      return;
    }

    try {
      const tx = await web3.eth.getTransaction(txHash);

      // Check if the transaction is valid and matches the specified address
      if (tx && tx.to && tx.to.toLowerCase() === address) {
        console.log('Incoming transaction detected:', tx);
      }
    } catch (err) {
      console.error('Error fetching transaction details:', err);
    }
  });

  // Handle process exit to unsubscribe
  process.on('SIGINT', () => {
    subscription.unsubscribe((err, success) => {
      if (success) {
        console.log('Successfully unsubscribed from pending transactions.');
      } else {
        console.error('Error unsubscribing:', err);
      }
      process.exit();
    });
  });
})();
