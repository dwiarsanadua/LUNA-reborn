--[[
  Quest 221: The History of War
  Level Required: 8
  NPC Start: 7, NPC Complete: 10
  Prerequisites: [220]
  Rewards: EXP=1300, Gold=2500
]]

local fsm = require('fsm_engine')

local quest_221 = fsm:new({
    id = 221,
    name = "The History of War",
    level_required = 8,
    npc_start = 7,
    npc_complete = 10,
    prerequisites = {220},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 221: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 221: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 221: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 221: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 221: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 221: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 221: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 221: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 221: Rewards given")
                self:give_rewards({
                    exp = 1300,
                    gold = 2500,
                    items = {
                        { item_id = 30000041, count = 3 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_221