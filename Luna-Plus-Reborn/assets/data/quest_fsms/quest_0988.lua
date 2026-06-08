--[[
  Quest 988: A 4th of July Barbeque!
  Level Required: 0
  NPC Start: 1, NPC Complete: 1
]]

local fsm = require('fsm_engine')

local quest_988 = fsm:new({
    id = 988,
    name = "A 4th of July Barbeque!",
    level_required = 0,
    npc_start = 1,
    npc_complete = 1,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 988: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 988: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 988: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 988: NPC talk objective met")
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
                self:log("Quest 988: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 988: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 988: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001068, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_988