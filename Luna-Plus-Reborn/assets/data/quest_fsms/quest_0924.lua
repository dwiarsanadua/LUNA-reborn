--[[
  Quest 924: [Quest] Rogue Hornet Queen
  Level Required: 101
  NPC Start: 540, NPC Complete: 540
  Rewards: EXP=8243986, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_924 = fsm:new({
    id = 924,
    name = "[Quest] Rogue Hornet Queen",
    level_required = 101,
    npc_start = 540,
    npc_complete = 540,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 924: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 924: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 924: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 333,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 924: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 924: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 924: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 924: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 924: Rewards given")
                self:give_rewards({
                    exp = 8243986,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_924