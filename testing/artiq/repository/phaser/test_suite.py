import os
import importlib
import importlib.util

"""Run a series to tests to validate sawg."""

def my_artiq_run(module_name):
    file_path = 'repository/phaser/'
    module = importlib.import_module( module_name )
    print('#' * 20)
    print(module)
    # print(module.SAWGTest.__doc__)
    s = 'artiq_run ' + file_path + '/' + module_name + '.py'
    repeat = True
    while(repeat):
        os.system(s)
        ans = input("Press ENTER to repeat test, 'n' for next test.")
        if ans == 'n':
            repeat = False

def main():

    for n in [1, 2, 3]:
        s = "test_simple_sin{:d}".format(n)
        my_artiq_run(s)

    my_artiq_run('test_reset')

    # test frequency jumps
    for s in ['00', '01', '02', '12']:
        test_name = "test_freqs{:s}".format(s)
        my_artiq_run(test_name)

    # test up conversion
    for n in [1, 2, 3]:
        s = "test_upconvert{:d}".format(n)
        my_artiq_run(s)

    # test absolute phase
    for n in [0, 1, 2, 3, 4]:
        s = "test_ap0{:d}".format(n)
        my_artiq_run(s)

    # test relative phase
    for n in [0, 1, 2]:
        s = "test_ap1{:d}".format(n)
        my_artiq_run(s)

    # test cancellation of sawg0.frequency0 and sawg0.frequency1
    my_artiq_run("test_ap20")



if __name__ == "__main__":
    main()
