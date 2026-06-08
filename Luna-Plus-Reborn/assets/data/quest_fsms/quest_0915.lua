--[[
  Quest 915: Uncle Greasy
  Level Required: 83
  NPC Start: 124, NPC Complete: 533
]]

local fsm = require('fsm_engine')

local quest_915 = fsm:new({
    id = 915,
    name = "Uncle Greasy",
    level_required = 83,
    npc_start = 124,
    npc_complete = 533,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 915: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 533,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 915: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 915: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 533,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 915: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 915: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 915: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 533,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 915: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 915: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_915