--[[
  Quest 277: [Emblem of Spirit] Stranded!
  Level Required: 70
  NPC Start: 70, NPC Complete: 70
  Rewards: EXP=420000, Gold=20000
]]

local fsm = require('fsm_engine')

local quest_277 = fsm:new({
    id = 277,
    name = "[Emblem of Spirit] Stranded!",
    level_required = 70,
    npc_start = 70,
    npc_complete = 70,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 277: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 277: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 277: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 277: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 277: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 277: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 70,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 277: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 277: Rewards given")
                self:give_rewards({
                    exp = 420000,
                    gold = 20000,
                    items = {
                        { item_id = 185, count = 25 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_277