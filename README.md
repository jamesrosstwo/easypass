# HackRPI
Secure, memorable passwords at a moment's notice.

## What it does
Unlike traditional password generators, EasyPass combines words from any one of 12 different languages into a mnemonic password of your chosen length in order to create passwords that are not only easy to remember but hard to crack due to their long length. The software also assists in saving the passwords in a local database securely encrypted by your Master Password. 

## How it's built
We wrote the software as a windows desktop application using win32, with all the source code written in c++. All the stored data are encrypted using the [Blowfish Encryption algorithm](https://en.wikipedia.org/wiki/Blowfish_(cipher)) and placed into .epass files inside your local directories. These can later be decrypted with a .key file - the master password to unlock your other passwords.

## What's next
In the future, we intend to add additional functionality, such as number substitution within the password, additional language support, and screen ratio adjustments for the GUI (anchor points) to allow for a wider variety of devices to use this software. We hope to implement sorting and searching through your accounts, copying and auto-typing the usernames and passwords.

 
