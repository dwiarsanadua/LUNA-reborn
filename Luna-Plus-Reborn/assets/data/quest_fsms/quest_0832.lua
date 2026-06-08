--[[
  Quest 832: Suppress Gwonjok 1
  Level Required: 78
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [413]
  Rewards: EXP=1850968, Gold=41009
]]

local fsm = require('fsm_engine')

local quest_832 = fsm:new({
    id = 832,
    name = "Suppress Gwonjok 1",
    level_required = 78,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {413},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 832: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 832: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 832: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 73,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 832: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 832: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 832: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 832: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 832: Rewards given")
                self:give_rewards({
                    exp = 1850968,
                    gold = 41009,
                })
            end,
            transitions = {},
        },
    },
})

return quest_832