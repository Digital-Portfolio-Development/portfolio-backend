import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`


async def test_project(service_client):
    response = await service_client.post(
        '/api/project',
        json={
            'name': 'hello',
            'short_description': 'project',
            'full_description': 'project for user portfolio',
            'tags': "prod,random,programming",
            'priority': 6,
            'visibility': True
        },
    )
    assert response.status == 401
    assert response.text == '{"message":"unauthorized"}'
