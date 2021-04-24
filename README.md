# ClauScript
    new project, Modified [ClauText](https://github.com/ClauText/ClauText) to use in other Project...

# Example 
    
    data = {

    }

    x = 5

    Main = {
        $call = { id = test } 
    }

    Event = {
        id = test2


        $return = { /../../x }
    }	

    Event = {
        id = test

        $query = {
            workspace = { $concat_all = { / . / data } }
            $insert = {
                @x = 15
                @y = {
                    z = 0
                } 
                @"a" = 3

                @provinces = {
                    -1 = {
                        x = 0
                    }
                    -2 = {
                        x = 1
                    }
                }
            }
            $insert = {
                x = 15

                provinces = {
                    $ = {
                        x = 0
                        @y = wow2
                    }
                }
            }
            $update = {
                #@x = 2 # @ : target, 2 : set value
                "a" = 3 # condition
                y = {
                    @z = 4 # @ : target.
                }
                provinces = {
                    $ = {
                        x = 0
                        @y = %event_test2
                    }
                }
            }
            $delete = {
                @x = 1 # @ : remove object., if value is 1 then remove
                "a" = 3 # condition.
                y = {
                    @z = %any # %any : condition - always.
                }
                provinces = {
                    @$ = { # $ : all usertype( array or object or mixed )
                        x = 1 # condition.
                    }
                }
            }
        }

        $print2 = { dir = { /./data } }
        $_getch = { }
    }

