from setuptools import setup, find_packages


if __name__ == '__main__':
    setup(
        name='python-artm',
        version='0.1.0',
        author='Anton Martsenyuk',
        author_email='antoniox@yandex.ru',
        packages=find_packages(exclude=['debian', 'tests']),
        include_package_data=True,
        zip_safe=False,
    )
