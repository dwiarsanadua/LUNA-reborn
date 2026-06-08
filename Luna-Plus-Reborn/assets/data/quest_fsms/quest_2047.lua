--[[
  Quest 2047: [Daily]Supporting Nera Harbor 3
  Level Required: 97
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=4133377
]]

local fsm = require('fsm_engine')

local quest_2047 = fsm:new({
    id = 2047,
    name = "[Daily]Supporting Nera Harbor 3",
    level_required = 97,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2047: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2047: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2047: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2047: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2047: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2047: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2047: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2047: Rewards given")
                self:give_rewards({
                    exp = 4133377,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2047