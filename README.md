# IRC Server

Un serveur **Internet Relay Chat (IRC)** implémenté en C++ avec support complet du protocole IRC et un bot intégré.

## 📋 Table des matières

- [À propos](#-à-propos)
- [Fonctionnalités](#-fonctionnalités)
- [Prérequis](#-prérequis)
- [Installation](#-installation)
- [Utilisation](#-utilisation)
- [Architecture](#-architecture)
- [Commandes IRC supportées](#-commandes-irc-supportées)
- [Bot TimeBot](#-bot-timebot)
- [Troubleshooting](#-troubleshooting)

## 🎯 À propos

Ce projet implémente un serveur IRC complet permettant à plusieurs clients de se connecter, d'échanger des messages en temps réel, de créer et gérer des canaux. Un bot TimeBot est fourni pour démontrer les capacités du serveur.

### Enjeux du projet

- **Gestion de connexions multiples** : Gérer plusieurs clients simultanément avec des sockets non-bloquants
- **Protocole IRC** : Respecter la RFC 1459/2812 du protocole IRC
- **Architecture modulaire** : Séparer les responsabilités (Client, Serveur, Channel)
- **Commandes IRC** : Implémenter les commandes essentielles du protocole
- **Bot autonome** : Créer un bot capable d'interagir avec le serveur

## ⚡ Fonctionnalités

✅ Serveur IRC multi-clients avec sockets non-bloquants  
✅ Gestion des canaux (#channel)  
✅ Authentification par mot de passe  
✅ Commandes IRC essentielles (JOIN, PART, PRIVMSG, NICK, etc.)  
✅ Système d'invitations et de contrôle d'accès  
✅ Bot TimeBot avec réponses automatiques  
✅ Gestion des signaux (SIGINT, SIGTERM)  

## 📦 Prérequis

- **Compilateur C++** : g++ ou clang (C++98 ou supérieur)
- **Make** : Pour compiler le projet
- **Système UNIX/Linux** : Pour les appels système (socket, signal)

## 🔧 Installation

### Compiler le serveur

```bash
make
```

### Nettoyer les fichiers compilés

```bash
make clean
```

### Réinitialiser et recompiler

```bash
make re
```

## 🚀 Utilisation

### Démarrer le serveur

```bash
./ircserv <port> <mot_de_passe>
```

**Paramètres :**
- `<port>` : Port d'écoute du serveur (ex: 6667)
- `<mot_de_passe>` : Mot de passe pour la connexion

**Exemple :**
```bash
./ircserv 6667 mypassword
```

### Démarrer le bot TimeBot

```bash
./bot <port> <mot_de_passe> [nick]
```

**Paramètres :**
- `<port>` : Port du serveur IRC
- `<mot_de_passe>` : Mot de passe du serveur
- `[nick]` : Surnom du bot (par défaut: TimeBot)

Le bot rejoint automatiquement le canal `#timebot`.

**Exemple :**
```bash
./bot 6667 mypassword TimeBot
```

### Se connecter en tant que client

Utilisez un client IRC standard (comme `nc`, `telnet`, ou un client graphique) :

```bash
nc localhost 6667
```

**Protocole de connexion :**
```
PASS mypassword
NICK mynick
USER mynick 0 * :Real Name
```

## 🏗️ Architecture

```
irc/
├── Makefile              # Script de compilation
├── includes/
│   ├── Client.hpp        # Classe représentant un client
│   └── Serveur.hpp       # Classe représentant le serveur
├── srcs/
│   ├── main.cpp          # Point d'entrée du serveur
│   ├── bot.cpp           # Bot TimeBot autonome
│   ├── Classes/
│   │   ├── Client.cpp    # Implémentation Client
│   │   ├── Channel.cpp   # Gestion des canaux
│   │   └── Serveur.cpp   # Implémentation Serveur
│   └── Commandes/
│       ├── JOIN.cpp      # Commande JOIN
│       ├── PART.cpp      # Commande PART (quitter un canal)
│       ├── PRIVMSG.cpp   # Commande PRIVMSG (message privé)
│       ├── NICK.cpp      # Commande NICK (changer de surnom)
│       ├── KICK.cpp      # Commande KICK (expulser un utilisateur)
│       ├── MODE.cpp      # Commande MODE (modes de canal)
│       ├── TOPIC.cpp     # Commande TOPIC (sujet du canal)
│       ├── INVITE.cpp    # Commande INVITE (inviter un utilisateur)
│       ├── USERandNICK.cpp # Authentification USER/NICK
│       ├── SIGNAUX.cpp   # Gestion des signaux
│       └── utils.cpp     # Fonctions utilitaires
```

## 🔌 Commandes IRC implémentées

| Commande | Description | Exemple |
|----------|-------------|---------|
| `PASS` | Authentification du serveur | `PASS mypassword` |
| `NICK` | Définir ou changer de surnom | `NICK mynick` |
| `USER` | Information utilisateur | `USER mynick 0 * :Real Name` |
| `JOIN` | Rejoindre un canal | `JOIN #general` |
| `PART` | Quitter un canal | `PART #general` |
| `PRIVMSG` | Envoyer un message | `PRIVMSG #general :Hello everyone!` |
| `KICK` | Expulser un utilisateur | `KICK #general user` |
| `MODE` | Changer les modes du canal | `MODE #general +o user` |
| `TOPIC` | Définir le sujet du canal | `TOPIC #general :New topic` |
| `INVITE` | Inviter un utilisateur | `INVITE user #general` |
| `QUIT` | Quitter le serveur | `QUIT` |
| `PING` | Vérifier la connexion | `PING` |

## 🤖 Bot TimeBot

TimeBot est un bot IRC autonome qui :

- ✅ Se connecte au serveur
- ✅ Rejoint automatiquement le canal `#timebot`
- ✅ Répond aux demandes de temps : répond `Current time:` si quelqu'un demande "time"
- ✅ Répond aux demandes de date : répond `Date:` si quelqu'un demande "date"
- ✅ Envoie des mises à jour automatiques toutes les 5 minutes
- ✅ Gère les signaux SIGINT/SIGTERM proprement

**Interaction avec TimeBot :**

Dans le canal IRC :
```
<user> !timebot time
<TimeBot> Current time: 2026-05-12 14:32:15

<user> !timebot date
<TimeBot> Date: Sunday, May 12 2026
```

## ⚠️ Troubleshooting

### "Address already in use"
Le port est déjà utilisé. Attendez quelques secondes ou utilisez un autre port :
```bash
./ircserv 6668 mypassword
```

### "Connection refused"
Assurez-vous que le serveur est en cours d'exécution sur le port spécifié.

### Le bot ne se connecte pas
Vérifiez :
- Le serveur est en cours d'exécution
- Le port est correct
- Le mot de passe est correct
- Pas de firewall bloquant la connexion

### Les messages ne s'affichent pas
Vérifiez que vous avez rejoint le même canal avec `JOIN #canalname`.

## 📝 Licence

Projet éducatif.

## 👤 Auteur

Développé comme projet d'apprentissage de la gestion d'un serveur réseau en C++.

---

**Besoin d'aide ?** Consultez le code source ou les commentaires dans les fichiers.
