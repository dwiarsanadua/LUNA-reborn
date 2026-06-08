--[[
  Quest 230: Message of Wisdom
  Level Required: 30
  NPC Start: 64, NPC Complete: 65
  Prerequisites: [226, 227, 228, 229]
  Rewards: EXP=18516, Gold=5000
]]

local fsm = require('fsm_engine')

local quest_230 = fsm:new({
    id = 230,
    name = "Message of Wisdom",
    level_required = 30,
    npc_start = 64,
    npc_complete = 65,
    prerequisites = {226, 227, 228, 229},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 230: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 230: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 230: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 230: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 230: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 230: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 230: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 230: Rewards given")
                self:give_rewards({
                    exp = 18516,
                    gold = 5000,
                    items = {
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_230