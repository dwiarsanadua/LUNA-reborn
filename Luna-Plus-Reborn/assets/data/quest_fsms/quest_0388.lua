--[[
  Quest 388: Master Hunter
  Level Required: 75
  NPC Start: 33, NPC Complete: 33
  Prerequisites: [351]
]]

local fsm = require('fsm_engine')

local quest_388 = fsm:new({
    id = 388,
    name = "Master Hunter",
    level_required = 75,
    npc_start = 33,
    npc_complete = 33,
    prerequisites = {351},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 388: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 388: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 388: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 388: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 388: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 388: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 388: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000504, count = 1 },
                        { item_id = 30000018, count = 1 },
                        { item_id = 30000021, count = 1 },
                        { item_id = 293, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_388