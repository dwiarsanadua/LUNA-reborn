--[[
  Quest 950: The Whereabouts of the Necklace
  Level Required: 107
  NPC Start: 540, NPC Complete: 540
  Rewards: EXP=15387160, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_950 = fsm:new({
    id = 950,
    name = "The Whereabouts of the Necklace",
    level_required = 107,
    npc_start = 540,
    npc_complete = 540,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 950: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 950: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 950: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 950: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 950: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 950: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 950: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 950: Rewards given")
                self:give_rewards({
                    exp = 15387160,
                    gold = 101632,
                    items = {
                        { item_id = 413, count = 1 },
                        { item_id = 413, count = 1 },
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_950