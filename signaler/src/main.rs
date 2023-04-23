use std::env;


fn main() {

    // Get all args from argv
    let mut args =  env::args_os();
    
    // Discard first argument, which is the executable path
    args.next().unwrap();

    // Parse PID and SIGNAL
    let pid = args.next().expect("Missing PID argument. (run as ./signaler <pid> <signal>)");
    let signal = args.next().expect("Missing SIGNAL argument. (run as ./signaler <pid> <signal>)");
   
    println!("PID: {}", pid.into_string().unwrap());
    println!("SIGNAL: {}", signal.into_string().unwrap());

}
