--[[
  Quest 278: [Emblem of Spirit] Stranded!
  Level Required: 70
  NPC Start: 70, NPC Complete: 70
  Prerequisites: [277]
  Rewards: EXP=420000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_278 = fsm:new({
    id = 278,
    name = "[Emblem of Spirit] Stranded!",
    level_required = 70,
    npc_start = 70,
    npc_complete = 70,
    prerequisites = {277},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 278: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 278: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 278: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 109,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 278: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 278: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 278: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 278: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 278: Rewards given")
                self:give_rewards({
                    exp = 420000,
                    gold = 20000,
                    items = {
                        { item_id = 186, count = 25 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_278