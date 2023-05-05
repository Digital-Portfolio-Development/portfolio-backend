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
            'password': '12345678'
        },
    )
    assert response.status == 201
    assert response.text == '{"message":"user was created!"}'


async def test_user_login(service_client):
    response = await service_client.post(
        '/api/user/login',
        json={
            'username': 'johndoe',
            'password': '12345678'
        },
    )
    assert response.status == 400
    assert response.text == '{"message":"Invalid email or password"}'


async def test_success_auth(service_client):
    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'password': '12345678'
        },
    )
    assert response.status == 201
    assert response.text == '{"message":"user was created!"}'

    response = await service_client.post(
        '/api/user/login',
        json={
            'username': 'johndoe',
            'password': '12345678'
        },
    )
    assert response.status == 200
    assert response.text == '{"message":"Success auth!"}'


async def test_db_updates(service_client):
    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'password': '12345678'
        },
    )
    assert response.status == 201
    assert response.text == '{"message":"user was created!"}'

    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'password': '12345678'
        },
    )
    assert response.status == 400
    assert response.text == '{"message":"this username is already in use"}'


@pytest.mark.pgsql('public', files=['create_user.sql'])
async def test_db_initial_data(service_client):
    response = await service_client.post(
        '/api/user/register',
        json={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'password': '12345678'
        },
    )
    assert response.status == 400
    assert response.text == '{"message":"this username is already in use"}'
