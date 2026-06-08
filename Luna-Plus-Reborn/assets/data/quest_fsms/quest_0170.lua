--[[
  Quest 170: Mirror, Mirror
  Level Required: 38
  NPC Start: 3, NPC Complete: 60
  Rewards: EXP=102067, Gold=5025
]]

local fsm = require('fsm_engine')

local quest_170 = fsm:new({
    id = 170,
    name = "Mirror, Mirror",
    level_required = 38,
    npc_start = 3,
    npc_complete = 60,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 170: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 170: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 170: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 170: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 170: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 170: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 170: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 170: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 170: Rewards given")
                self:give_rewards({
                    exp = 102067,
                    gold = 5025,
                    items = {
                        { item_id = 84, count = 1 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_170