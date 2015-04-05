'use continuation'
mongoose = require("../utils/db")

UserSchema = new mongoose.Schema(
  name:
    type: String
    index: true
    unique: true
  identity: [ Buffer ]
)

mongoose.model "User", UserSchema
module.exports = User = mongoose.model "User"

UserSchema.pre 'save', (next) ->
  @identity ?= []

  next()

User.checkName = (name, callback) ->
  User.findOne name: name, (err, user) ->
    return callback(err) if err
    if user
      callback "username-occupied"
    else
      callback null

User.checkIdentity = (identity, callback) ->
  User.getByIdentity identity, (err, user) ->
    if err is 'no-such-user'
      callback null
    else if err
      callback err
    else
      callback 'identity-exist'

User.getByName = (name, callback) ->
  User.findOne name: name, (err, user) ->
    return callback(err) if err
    if not user
      callback 'no-such-user'
    else
      callback null, user

User.getByIdentity = (identity, callback) ->
  User.findOne identity: $elemMatch: $eq: identity, (err, user) ->
    return callback(err) if err
    if not user
      callback 'no-such-user'
    else
      callback null, user

# Create a new user
User.create = (user, callback) ->
  try
    # Normalize username and email address to lower case
    user.name = user.name.toLowerCase()
    usernameRegex = /^[a-z][a-z0-9_]{3,11}$/
    if not usernameRegex.exec(user.name)
      return callback("invalid-username")
    user = new User(user)
    callback null, user
  catch err
    callback err


User::addIdentity = (identity, callback) ->
  @identity ?= []
  self = this
  try
    User.checkIdentity(identity, obtain())
    self.identity.push identity
    self.save callback
  catch err
    callback err

User::removeIdentity = (identity, callback) ->
  i = 0
  while i < @identity.length
    if @identity[i].equals identity
      @identity = @identity.slice(0, i).concat(@identity.slice(i + 1))
      @save callback
      return
    i++
  callback "no-such-identity"

User.getOrCreateByName = (name, callback) ->
  try
    User.getByName(name, obtain(user))
    callback null, user
  catch err
    try
      if err is 'no-such-user'
        User.create({name: name}, obtain(user))
        callback null, user
      else
        throw err
    catch err2
      callback err2
