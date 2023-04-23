use std::env;


fn main() {

    for argument in env::args_os() {
        
        println!("{}", 
            argument.into_string().expect("Error reading arguments.")
        );
    }
}
