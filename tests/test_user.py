import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`


async def test_user_register(service_client):
    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'email': 'johndoe@gmail.com',
            'password': '12345'
        },
    )
    assert response.status == 201
    assert response.text == 'User johndoe was created!\n'


async def test_user_login(service_client):
    response = await service_client.post(
        '/api/user/login',
        json={
            'email': 'example@gmail.com',
            'password': 'example'
        },
    )
    assert response.status == 400
    assert response.text == 'Invalid email or password\n'


async def test_success_auth(service_client):
    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'email': 'johndoe@gmail.com',
            'password': '12345'
        },
    )
    assert response.status == 201
    assert response.text == 'User johndoe was created!\n'

    response = await service_client.post(
        '/api/user/login',
        json={
            'email': 'johndoe@gmail.com',
            'password': '12345'
        },
    )
    assert response.status == 200
    assert response.text == 'Success auth!\n'


async def test_db_updates(service_client):
    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'email': 'johndoe@gmail.com',
            'password': '12345'
        },
    )
    assert response.status == 201
    assert response.text == 'User johndoe was created!\n'

    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'email': 'johndoe@gmail.com',
            'password': '12345'
        },
    )
    assert response.status == 400
    assert response.text == 'User with this username already exists\n'


@pytest.mark.pgsql('public', files=['create_user.sql'])
async def test_db_initial_data(service_client):
    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'email': 'johndoe@gmail.com',
            'password': '12345'
        },
    )
    assert response.status == 400
    assert response.text == 'User with this username already exists\n'
