from setuptools import find_packages, setup

package_name = 'my_python_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='huy',
    maintainer_email='huy@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            "quanghuy_node = my_python_pkg.firstnode:main", 
            "publisher_node = my_python_pkg.mypub:main",
            "subcriber_node = my_python_pkg.mysub:main"
        ],
    },
)
