--[[
  Quest 2034: [Daily]Paid for Supplies 2
  Level Required: 84
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=1794522
]]

local fsm = require('fsm_engine')

local quest_2034 = fsm:new({
    id = 2034,
    name = "[Daily]Paid for Supplies 2",
    level_required = 84,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2034: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2034: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2034: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2034: NPC talk objective met")
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
                self:log("Quest 2034: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2034: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2034: Rewards given")
                self:give_rewards({
                    exp = 1794522,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2034