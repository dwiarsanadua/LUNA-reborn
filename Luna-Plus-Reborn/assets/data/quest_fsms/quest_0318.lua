--[[
  Quest 318: [Special] Obtain B Class License
  Level Required: 45
  NPC Start: 33, NPC Complete: 33
  Prerequisites: [317]
]]

local fsm = require('fsm_engine')

local quest_318 = fsm:new({
    id = 318,
    name = "[Special] Obtain B Class License",
    level_required = 45,
    npc_start = 33,
    npc_complete = 33,
    prerequisites = {317},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 318: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 318: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 318: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 318: NPC talk objective met")
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
                self:log("Quest 318: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 318: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 318: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000500, count = 1 },
                        { item_id = 224, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_318