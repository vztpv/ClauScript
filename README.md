# ClauScript
    new project, Simple ClauText to use in JsonLint Project...


# ToDo
    change ToBool3, ToBool4...
    /~~ or $parameter.~~ or $local.~~   =  value

    x = 3   y = 5 # 

    /./x = 3
    /./x = /./y # calcul /./y
    /./x = '/./y' # no calcul and ( x <= /./y )
    $add = { $unwrap = { '/./y' } /./x }
