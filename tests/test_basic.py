import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`


async def test_first_time_users(service_client):
    response = await service_client.post(
        '/user/register',
        params={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe',
            'email': 'johndoe@gmail.com',
            'password': '12345',
        },
    )
    assert response.status == 201
    assert response.text == 'User johndoe was created!\n'


async def test_db_updates(service_client):
    response = await service_client.post(
        '/user/register',
        params={
            'name': 'John',
            'last_name': 'Doe',
            'username': 'johndoe2',
            'email': 'johndoe@gmail.com',
            'password': '12345',
        },
    )
    assert response.status == 201
    assert response.text == 'User johndoe2 was created!\n'


# @pytest.mark.pgsql('db_1', files=['initial_data.sql'])
# async def test_db_initial_data(service_client):
#     response = await service_client.post(
#         '/v1/hello',
#         params={'name': 'user-from-initial_data.sql'},
#     )
#     assert response.status == 200
#     assert response.text == 'Hi again, user-from-initial_data.sql!\n'
