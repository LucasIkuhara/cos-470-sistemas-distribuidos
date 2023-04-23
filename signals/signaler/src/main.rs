use std::{env, process::Command};
use sysinfo::{Pid, SystemExt, ProcessExt};


fn main() {

    // Get all args from argv
    let mut args =  env::args_os();
    
    // Discard first argument, which is the executable path
    args.next().unwrap();

    // Parse PID and SIGNAL
    let pid = args.next()
        .expect("Missing PID argument. (run as ./signaler <pid> <signal>)")
        .into_string().unwrap();
        // .parse().expect("The received value for PID cannot be cast to int. Please use numbers only.");

    let signal = args.next()
        .expect("Missing SIGNAL argument. (run as ./signaler <pid> <signal>)")
        .into_string().unwrap();
        // .parse().expect("The received value for SIGNAL cannot be cast to int. Please use numbers only.");

    println!("PID: {}", pid);
    println!("SIGNAL: {}", signal);

    // Get and update system info
    let mut system = sysinfo::System::new_all();
    system.refresh_all();

    // Get PID as a PID Struct
    let pid_number: usize = pid.parse().expect("The received value for PID cannot be cast to int. Please use numbers only.");
    let pid_struct: Pid = Pid::from(pid_number);

    // Confirm the PID exists
    let proc = system.process(pid_struct).expect("Chosen PID does not exist.");
    println!("PID {} exists and is {}", pid, proc.status());
    
    // Send signal using the "kill" command
    print!("Dispatching signal {} to the process with PID={}\n", signal, pid);
    let mut kill = Command::new("kill")
        .args(["-s", signal.as_str(), pid.as_str()])
        .spawn().expect("Failed to dispatch signal.");

    kill.wait().unwrap();

}
