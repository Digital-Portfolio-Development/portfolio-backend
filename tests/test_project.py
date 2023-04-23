import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`


async def test_project(service_client):
    response = await service_client.post(
        '/api/project',
        json={
            'name': 'hello',
            'description': 'project for user portfolio'
        },
    )
    assert response.status == 401
    assert response.text == 'Unauthorized\n'
