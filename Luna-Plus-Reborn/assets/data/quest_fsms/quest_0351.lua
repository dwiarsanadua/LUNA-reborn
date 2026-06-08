--[[
  Quest 351: [Special] Obtain A Class License
  Level Required: 61
  NPC Start: 33, NPC Complete: 33
  Prerequisites: [318]
]]

local fsm = require('fsm_engine')

local quest_351 = fsm:new({
    id = 351,
    name = "[Special] Obtain A Class License",
    level_required = 61,
    npc_start = 33,
    npc_complete = 33,
    prerequisites = {318},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 351: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 351: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 351: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 351: NPC talk objective met")
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
                self:log("Quest 351: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 351: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 351: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000502, count = 1 },
                        { item_id = 257, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_351