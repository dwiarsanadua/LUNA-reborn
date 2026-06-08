--[[
  Quest 122: Silence the Sound
  Level Required: 25
  NPC Start: 23, NPC Complete: 27
  Prerequisites: [121]
  Rewards: EXP=8697, Gold=500
]]

local fsm = require('fsm_engine')

local quest_122 = fsm:new({
    id = 122,
    name = "Silence the Sound",
    level_required = 25,
    npc_start = 23,
    npc_complete = 27,
    prerequisites = {121},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 122: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 122: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 122: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 23,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 122: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 122: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 122: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 122: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 122: Rewards given")
                self:give_rewards({
                    exp = 8697,
                    gold = 500,
                    items = {
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_122