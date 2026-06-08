--[[
  Quest 748: [Quest]History of the Contonet
  Level Required: 2
  NPC Start: 9, NPC Complete: 47
  Rewards: EXP=12
]]

local fsm = require('fsm_engine')

local quest_748 = fsm:new({
    id = 748,
    name = "[Quest]History of the Contonet",
    level_required = 2,
    npc_start = 9,
    npc_complete = 47,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 748: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 47,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 748: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 748: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 47,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 748: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 748: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 748: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 47,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 748: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 748: Rewards given")
                self:give_rewards({
                    exp = 12,
                    items = {
                        { item_id = 21001516, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_748