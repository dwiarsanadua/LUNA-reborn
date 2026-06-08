--[[
  Quest 158: Gift Giving
  Level Required: 31
  NPC Start: 5, NPC Complete: 29
  Prerequisites: [157]
  Rewards: EXP=46658, Gold=3945
]]

local fsm = require('fsm_engine')

local quest_158 = fsm:new({
    id = 158,
    name = "Gift Giving",
    level_required = 31,
    npc_start = 5,
    npc_complete = 29,
    prerequisites = {157},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 158: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 158: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 158: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 158: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 158: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 158: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 158: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 158: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 158: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 158: Rewards given")
                self:give_rewards({
                    exp = 46658,
                    gold = 3945,
                    items = {
                        { item_id = 72, count = 1 },
                        { item_id = 73, count = 1 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_158