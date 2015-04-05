'use continuation'

config = require "../config"
User = require "../models/user"

module.exports = (fingerPrint, callback) ->
  try
    User.getByIdentity(fingerPrint, obtain(user))
    callback null, user.name
  catch err
    if err is 'no-such-user'
      callback null, null
    else
      callback err